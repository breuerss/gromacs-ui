#include "simulationstatuschecker.h"
#include "command/fileobject.h"
#include "model/project.h"
#include "pipeline/simulation/configuration.h"
#include "pipeline/simulation/filenamegenerator.h"
#include "appprovider.h"

#include <QFile>
#include <QProcess>
#include <QVariant>
#include <memory>

const QString SimulationStatusChecker::potentialCommand(
  "awk '/ Potential / "
  " {for(i=1;i<length;i+=15){ a=substr($0,i,15);"
  " if (a ~ /Potential/)"
  " { getline; print substr($0,i,15)}}}' %1"
  );

SimulationStatusChecker::SimulationStatusChecker(
  std::shared_ptr<Model::Project> project,
  const Pipeline::Simulation::Configuration* simulation,
  QObject *parent
  )
  : QObject(parent)
  , project(project)
  , simulation(simulation)
  , fileNameGenerator(std::make_unique<Pipeline::Simulation::FileNameGenerator>(project))
{
  fileNameGenerator->setConfiguration(simulation);
}

QString SimulationStatusChecker::getLogPath() const
{
  return fileNameGenerator->getFileNameFor(Command::FileObject::Type::LOG);
}

bool SimulationStatusChecker::logPathExists() const
{
  return QFile(getLogPath()).exists();
}

QList<float> SimulationStatusChecker::getProgressValues() const
{
  QList<float> values;
  if (!logPathExists())
  {
    return values;
  }

  QProcess check;
  QString command(potentialCommand);
  QString logPath = getLogPath();
  check.start("bash", QStringList() << "-c" << command.arg(logPath));
  check.waitForFinished();
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
  if (!logPathExists())
  {
    return std::numeric_limits<float>::quiet_NaN();
  }

  QString command = QString("awk '/Step/ { getline; print $1}' %1 | tail -n1");
  if (simulation->isMinimisation())
  {
    command = potentialCommand + " | tail -n1";
  }

  QProcess check;
  QString logPath = getLogPath();
  check.start("bash", QStringList() << "-c" << command.arg(logPath));

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
