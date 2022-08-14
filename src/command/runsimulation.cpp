#include "runsimulation.h"
#include "../gromacsconfigfilegenerator.h"
#include "../statusmessagesetter.h"
#include "../model/systemsetup.h"
#include "../model/project.h"
#include "../model/simulation.h"
#include "../appprovider.h"
#include "../logforwarder.h"
#include "../simulationstatuschecker.h"
#include "qfilesystemwatcher.h"

#include <QDebug>
#include <QDir>

namespace Command {

RunSimulation::RunSimulation(
  std::shared_ptr<Model::Project> project,
  std::shared_ptr<Model::Simulation> newSimulation,
  QObject *parent)
  : Executor(parent)
    , project(project)
    , simulation(newSimulation)
{
  connect(this, &RunSimulation::finished, [this] () {
    progressChecker.removePath(simulationChecker->getLogPath());
  });
  connect(this, &RunSimulation::started, [this] () {
    emit progress(0, simulation->isMinimisation() ? ProgressType::Value : ProgressType::Percentage);
  });
  connect(&progressChecker, &QFileSystemWatcher::fileChanged, this, &RunSimulation::checkProgress);
}

void RunSimulation::doExecute()
{
  qDebug() << "Exec simulation";
  QString gmx = AppProvider::get("gmx");
  if (gmx.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  simulationChecker = std::make_shared<SimulationStatusChecker>(project, simulation);
  QString mdpFile = simulationChecker->getMdpPath();
  QFileInfo fi(mdpFile);
  QDir dir(fi.absolutePath());
  dir.mkpath(".");

  GromacsConfigFileGenerator::generate(simulation, mdpFile);

  QString inputStructure = project->getSystemSetup()->getProcessedStructureFile();
  QFileInfo systemPath(inputStructure);
  if (simulation->getPreviousStep())
  {
    SimulationStatusChecker prevSimChecker(project, simulation->getPreviousStep());
    inputStructure = prevSimChecker.getCoordinatesPath();
  }

  if (!execGrompp(
      mdpFile,
      inputStructure,
      systemPath.absolutePath() + "/topol.top",
      simulationChecker->getTprPath(),
      dir.absolutePath()
      ))
  {
    return;
  }

  QString command = gmx + " mdrun";
  command += " -deffnm " + simulation->getTypeAsString();
  command += " -cpt 5";
  if (simulation->property("resume").value<bool>())
  {
    command += " -cpi";
  }

  StatusMessageSetter::getInstance()->setMessage("Executing " + command);
  process.setWorkingDirectory(dir.absolutePath());
  process.start(command);

  checkProgress();
}

QString RunSimulation::getName() const
{
  return "mdrun simulation";
}

bool RunSimulation::execGrompp(
  const QString& mdpFile,
  const QString& inputStructure,
  const QString& topology,
  const QString& output,
  const QString& workingDirectory
  )
{
  QString command = AppProvider::get("gmx") + " grompp";
  command += " -f " + mdpFile;
  command += " -c " + inputStructure;
  command += " -p " + topology;
  command += " -maxwarn 2 ";
  command += " -o " + output;

  qDebug() << "executing grompp" << command;
  QProcess grompp;
  LogForwarder::getInstance()->listenTo(&grompp);
  grompp.setWorkingDirectory(workingDirectory);

  grompp.start(command);
  grompp.waitForFinished();

  const bool successful = grompp.exitCode() == 0;
  if (!successful)
  {
    qDebug() << "error executing grompp";
    StatusMessageSetter::getInstance()->setMessage("Could not execute " + command);
  }

  LogForwarder::getInstance()->detach(&grompp);
  return successful;
}

void RunSimulation::checkProgress()
{
  QString logPath = simulationChecker->getLogPath();

  QFile logFile(logPath);
  if (!logFile.exists())
  {
    // file path needs to exist to being watched.
    logFile.open(QFile::WriteOnly);
    logFile.close();
  }

  if (!progressChecker.files().contains(logPath))
  {
    // needs reconnection since on the first start of
    // simulation the logfile is removed and recreated
    // by mdrun. So the first addition to watch is
    // shifted inside of this function as well.
    progressChecker.addPath(logPath);
  }

  QString command = QString("awk '/Step/ { getline; print $1}' %1 | tail -n1");
  if (simulation->isMinimisation())
  {
    command = QString("tail -n30 %1 | awk '/Potential/ { getline; print $1}' | tail -n1");
  }

  QProcess check;
  check.start("bash", QStringList() << "-c" << command.arg(logPath));

  check.waitForFinished();
  QString stepsText = check.readAllStandardOutput();
  bool ok;
  float stepsDone = stepsText.trimmed().toFloat(&ok);
  if (!ok)
  {
    return;
  }

  if (simulation->isMinimisation())
  {
    emit progress(stepsDone, ProgressType::Value);
    return;
  }

  double numberOfSteps = simulation->property("numberOfSteps").value<double>();
  if (numberOfSteps > 0)
  {
    float progressValue = 100.0 * stepsDone / numberOfSteps;
    emit progress(progressValue);
  }
}

}
