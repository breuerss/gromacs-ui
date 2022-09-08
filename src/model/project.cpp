#include "project.h"
#include "systemsetup.h"
#include "../settings.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <memory>
#include "../pipeline/stepfactory.h"

namespace Model {

Project::Project()
  : QObject(nullptr)
  , systemSetup(new SystemSetup())
{
}

void Project::addStep(StepPointer&& step)
{
  pipelineSteps.push_back(step);
  emit stepAdded(step, pipelineSteps.size() - 1);
}

void Project::removeStep(int at)
{
  auto step = pipelineSteps[at];
  pipelineSteps.erase(pipelineSteps.begin() + at);
  emit stepRemoved(step, at);
}

void Project::clearSteps()
{
  while (pipelineSteps.size())
  {
    removeStep(0);
  }
}

const Project::StepPointerVector& Project::getSteps() const
{
  return pipelineSteps;
}

std::shared_ptr<SystemSetup> Project::getSystemSetup() const
{
  return systemSetup;
}

QString Project::getProjectPath()
{
  Settings settings;
  auto defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/gromacs-ui";
  auto projectsPath = settings.value(Settings::PROJECTS_DIRECTORY, defaultPath).toString();
  return projectsPath + "/" + name;
}

bool Project::initProjectDir(const QString& subDir)
{
  QDir dir(getProjectPath());
  return dir.mkpath(subDir);
}

QDataStream &operator<<(QDataStream &out, const std::shared_ptr<Project> project)
{
  out << project->property("name").value<QString>();
  out << *(project->getSystemSetup().get());

  int numberOfSteps = project->getSteps().size();
  out << numberOfSteps;

  qDebug() << numberOfSteps;

  QList<QList<int>> connections;

  const auto& steps = project->getSteps();
  for (int stepIndex = 0; stepIndex < steps.size(); stepIndex++)
  {
    const auto& step = steps[stepIndex];
    out << step->getType();
    out << step;

    for (const auto& fileObject : step->getFileObjectConsumer()->getConnectedTo().values())
    {

      for (int sourceNodeIndex = 0; sourceNodeIndex < steps.size(); sourceNodeIndex++)
      {
        const auto& sourceStep = steps[sourceNodeIndex];
        int found = sourceStep->getFileObjectProvider()->provides().indexOf(fileObject);
        if (found > -1)
        {
          auto triple = QList({stepIndex, sourceNodeIndex, found});
          connections << triple;
          break;
        }
      }
    }
  }

  out << connections;

  return out;
}

QDataStream &operator>>(QDataStream &in, std::shared_ptr<Project> project)
{
  QString name;
  in >> name;
  project->setProperty("name", name);

  in >> *(project->getSystemSetup().get());

  int numberOfSteps;
  in >> numberOfSteps;

  qDebug() << numberOfSteps;

  project->clearSteps();

  for (int stepIndex = 0; stepIndex < numberOfSteps; ++stepIndex)
  {
    QString type;
    in >> type;

    Pipeline::Step::Pointer step = Pipeline::StepFactory::getInstance()
      ->createFromString(type, project);
    in >> step;
    project->addStep(std::move(step));
  }

  QList<QList<int>> connections;
  in >> connections;

  for (const auto& conn: connections)
  {
    const int targetIndex = conn[0];
    const int sourceIndex = conn[1];
    const int sourcePort = conn[2];

    auto& steps = project->getSteps();
    steps[targetIndex]->getFileObjectConsumer()
      ->connectTo(steps[sourceIndex]->getFileObjectProvider()->provides()[sourcePort]);

  }

  return in;
}

}
