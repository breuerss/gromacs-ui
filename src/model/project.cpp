#include "project.h"
#include "systemsetup.h"
#include "../settings.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <memory>
#include <QJsonObject>
#include <QJsonArray>
#include "../pipeline/stepfactory.h"
#include "../command/fileobjectconsumer.h"
#include "../command/fileobjectprovider.h"

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

void Project::removeStep(std::shared_ptr<Pipeline::Step> step)
{
  auto it = std::remove(pipelineSteps.begin(), pipelineSteps.end(), step);
  int at = std::distance(pipelineSteps.begin(), it);
  pipelineSteps.erase(it, pipelineSteps.end());
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

QJsonObject &operator<<(QJsonObject& out, const std::shared_ptr<Project> project)
{
  out["name"] = project->property("name").toString();

  QJsonArray connections;
  QJsonArray stepsArray;
  const auto& steps = project->getSteps();
  for (size_t stepIndex = 0; stepIndex < steps.size(); stepIndex++)
  {
    const auto& step = steps[stepIndex];
    QJsonObject stepObject;
    stepObject << step;
    stepsArray.append(stepObject);

    for (const auto& fileObject : step->getFileObjectConsumer()->getConnectedTo().values())
    {
      for (size_t sourceNodeIndex = 0; sourceNodeIndex < steps.size(); sourceNodeIndex++)
      {
        const auto& sourceStep = steps[sourceNodeIndex];
        int found = sourceStep->getFileObjectProvider()->provides().indexOf(fileObject);
        if (found > -1)
        {
          auto triple = QJsonArray({
            (int)stepIndex, (int)sourceNodeIndex, found
          });
          connections.append(triple);
          break;
        }
      }
    }
  }

  out["steps"] = stepsArray;
  out["connections"] = connections;

  return out;
}

QJsonObject &operator>>(QJsonObject &in, std::shared_ptr<Project> project)
{
  project->setProperty("name", in["name"].toString());

  project->clearSteps();

  if (in.contains("steps") && in["steps"].isArray())
  {
    QJsonArray steps = in["steps"].toArray();
    for (int index = 0; index < steps.size(); index++)
    {
      QJsonObject jsonStep = steps[index].toObject();
      QString type = jsonStep["type"].toString();
      Pipeline::Step::Pointer step = Pipeline::StepFactory::getInstance()
        ->createFromString(type, project);

      jsonStep >> step;
      project->addStep(std::move(step));
    }
  }

  if (in.contains("connections") && in["connections"].isArray())
  {
    auto& steps = project->getSteps();
    QJsonArray connections = in["connections"].toArray();
    for (int index = 0; index < connections.size(); index++)
    {
      QJsonArray connection = connections[index].toArray();
      const int targetIndex = connection[0].toInt();
      const int sourceIndex = connection[1].toInt();
      const int sourcePort = connection[2].toInt();

      steps[targetIndex]->getFileObjectConsumer()
        ->connectTo(steps[sourceIndex]->getFileObjectProvider()->provides()[sourcePort]);
    }
  }

  return in;
}

}
