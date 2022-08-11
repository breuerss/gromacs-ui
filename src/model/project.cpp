#include "project.h"
#include "systemsetup.h"
#include "simulation.h"
#include "../settings.h"
#include <QDebug>
#include <QStandardPaths>
#include <memory>

namespace Model {

Project::Project()
  : systemSetup(new SystemSetup())
{
}

std::shared_ptr<Simulation> Project::addStep()
{
  auto step = std::make_shared<Simulation>();
  if (steps.size() >= 1)
  {
    step->setPreviousStep(steps.back());
  }
  steps.push_back(step);
  emit stepAdded(step, steps.size() - 1);
  return step;
}

void Project::removeStep(int at)
{
  auto step = steps[at];
  steps.erase(steps.begin() + at);
  if (steps.size() >= static_cast<size_t>(at))
  {
    std::shared_ptr<Simulation> previous;
    if (at > 1)
    {
      previous = steps[at - 1];
    }
    steps[at]->setPreviousStep(previous);
  }
  emit stepRemoved(step, at);
}

void Project::clearSteps()
{
  while (steps.size())
  {
    removeStep(0);
  }
}

const std::vector<std::shared_ptr<Simulation>>& Project::getSteps() const
{
  return steps;
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

  for (auto step: project.getSteps())
  {
    out << *(step.get());
  }

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

  for (int stepIndex = 0; stepIndex < numberOfSteps; ++stepIndex)
  {
    project.addStep();
  }

  for (auto step: project.getSteps())
  {
    qDebug() << "reading step";
    in >> *(step.get());
  }

  return in;
}

}
