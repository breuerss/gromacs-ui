#include "simulationstatuschecker.h"
#include "model/project.h"
#include "model/simulation.h"

#include <QFile>
#include <QProcess>
#include <QVariant>

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
  return hasCoordinates();
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

static const QString potentialCommand("awk '/ Potential / {for(i=1;i<length;i+=15){ a=substr($0,i,15); if (a ~ /Potential/) { getline; print substr($0,i,15)}}}'");

QList<float> SimulationStatusChecker::getProgressValues() const
{
  QProcess check;
  QString command(potentialCommand);
  check.start("bash", QStringList() << "-c" << command.arg(getLogPath()));
  check.waitForFinished();
  QList<float> values;
  while (check.canReadLine())
  {
    QString valueString = check.readLine();
    bool ok;
    float statusNumber = valueString.trimmed().toFloat(&ok);
    if (ok)
    {
      values << statusNumber;
    }
  }

  return values;
}

float SimulationStatusChecker::getProgress() const
{
  QString command = QString("awk '/Step/ { getline; print $1}' %1 | tail -n1");
  if (simulation->isMinimisation())
  {
    command = potentialCommand + " | tail -n1";
  }

  QProcess check;
  check.start("bash", QStringList() << "-c" << command.arg(getLogPath()));

  check.waitForFinished();
  QString valueString = check.readAllStandardOutput();
  bool ok;
  float statusNumber = valueString.trimmed().toFloat(&ok);
  if (!ok)
  {
    return std::numeric_limits<float>::quiet_NaN();
  }

  if (simulation->isMinimisation())
  {
    return statusNumber;
  }

  double numberOfSteps = simulation->property("numberOfSteps").value<double>();
  if (numberOfSteps > 0)
  {
    return 100.0 * statusNumber / numberOfSteps;
  }

  return std::numeric_limits<float>::quiet_NaN();
}
