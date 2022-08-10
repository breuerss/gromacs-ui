#include "simulationstatuschecker.h"
#include "model/project.h"
#include "model/simulation.h"

#include <QFile>

SimulationStatusChecker::SimulationStatusChecker(
  std::shared_ptr<Model::Project> project,
  std::shared_ptr<Model::Simulation> simulation,
  QObject *parent
  )
  : QObject(parent)
    , project(project)
    , simulation(simulation)
{
}

bool SimulationStatusChecker::hasData() const
{
  return hasCoordinates() || hasTrajectory();
}

bool SimulationStatusChecker::hasTrajectory() const
{
  return QFile(getTrajectoryPath()).exists();
}

bool SimulationStatusChecker::hasCoordinates() const
{
  return QFile(getCoordinatesPath()).exists();
}

bool SimulationStatusChecker::hasLog() const
{
  return QFile(getLogPath()).exists();
}

bool SimulationStatusChecker::hasMdp() const
{
  return QFile(getMdpPath()).exists();
}

QString SimulationStatusChecker::getTrajectoryPath() const
{
  return getBasePath() + ".xtc";
}

QString SimulationStatusChecker::getCoordinatesPath() const
{
  return getBasePath() + ".gro";
}

QString SimulationStatusChecker::getLogPath() const
{
  return getBasePath() + ".log";
}

QString SimulationStatusChecker::getMdpPath() const
{
  return getBasePath() + ".mdp";
}

QString SimulationStatusChecker::getTprPath() const
{
  return getBasePath() + ".tpr";
}

QString SimulationStatusChecker::getBasePath() const
{
  QString projectPath = project->getProjectPath();
  QString simulationType = simulation->getTypeAsString();
  return projectPath + "/" + simulationType + "/" + simulationType;
}
