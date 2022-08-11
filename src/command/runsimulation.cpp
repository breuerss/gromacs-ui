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
  int stepIndex,
  QObject *parent)
  : Executor(parent)
    , project(project)
    , stepIndex(stepIndex)
{
  connect(this, &RunSimulation::finished, [this] () {
    progressChecker.removePath(simulationChecker->getLogPath());
  });
  connect(this, &RunSimulation::started, [this] () {
    emit progress(0);
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

  const auto& steps = project->getSteps();
  std::shared_ptr<Model::Simulation> currentSim = steps[stepIndex];

  simulationChecker = std::make_shared<SimulationStatusChecker>(project, currentSim);
  QString mdpFile = simulationChecker->getMdpPath();
  QFileInfo fi(mdpFile);
  QDir dir(fi.absolutePath());
  dir.mkpath(".");

  GromacsConfigFileGenerator::generate(currentSim, mdpFile);

  QString inputStructure = project->getSystemSetup()->getNeutralisedStructureFile();
  QFileInfo systemPath(inputStructure);
  if (stepIndex > 0)
  {
    auto prevStep = steps[stepIndex - 1];
    SimulationStatusChecker prevSimChecker(project, prevStep);
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

  QString command = gmx + " mdrun -v";
  command += " -deffnm " + currentSim->getTypeAsString();
  command += " -cpt 5";
  command += " -cpi";

  StatusMessageSetter::getInstance()->setMessage("Executing " + command);
  process.setWorkingDirectory(dir.absolutePath());
  process.start(command);

  progressChecker.addPath(simulationChecker->getLogPath());
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
  QProcess check;
  QString command = QString("awk '/Step/ { getline; print $1}' %1 | tail -n1")
    .arg(simulationChecker->getLogPath());
  check.start("bash", QStringList() << "-c" << command);

  check.waitForFinished();
  QString stepsText = check.readAllStandardOutput();
  bool ok;
  int steps = stepsText.toInt(&ok, 10);
  if (ok)
  {
    const auto& simulations = project->getSteps();
    std::shared_ptr<Model::Simulation> currentSim = simulations[stepIndex];
    float progressValue = 100.0 * steps / currentSim->property("numberOfSteps").value<double>();
    emit progress(progressValue);
  }

}


}
