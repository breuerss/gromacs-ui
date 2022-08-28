#include "project.h"
#include "src/command/runsimulation.h"
#include "systemsetup.h"
#include "../config/simulation.h"
#include "../settings.h"
#include <QDebug>
#include <QStandardPaths>
#include <memory>

namespace Model {

Project::Project()
  : QObject(nullptr)
  , systemSetup(new SystemSetup())
{
}

/**
 * TODO
 *
 *  Unify Command and Simulation
 *  Split up SystemSetup to series of Pipeline Nodes
 */

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

const std::vector<Pipeline::StepType>& Project::getSteps() const
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

QDataStream &operator<<(QDataStream &out, const Project &project)
{
  out << project.property("name").value<QString>();
  out << *(project.getSystemSetup().get());

  int numberOfSteps = project.getSteps().size();
  out << numberOfSteps;

  //for (auto step: project.getSteps())
  //{
  //  out << *(step.get());
  //}

  return out;
}

QDataStream &operator>>(QDataStream &in, Project &project)
{
  QString name;
  in >> name;
  project.setProperty("name", name);

  in >> *(project.getSystemSetup().get());

  int numberOfSteps;
  in >> numberOfSteps;

  while (project.getSteps().size())
  {
    project.removeStep(project.getSteps().size() - 1);
  }

  //for (int stepIndex = 0; stepIndex < numberOfSteps; ++stepIndex)
  //{
  //  project.addStep();
  //}

  //for (auto step: project.getSteps())
  //{
    //qDebug() << "reading step";
    //in >> *(step.get());
  //}

  return in;
}

}
