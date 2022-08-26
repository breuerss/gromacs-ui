#include "runsimulation.h"
#include "../gromacsconfigfilegenerator.h"
#include "../statusmessagesetter.h"
#include "../model/systemsetup.h"
#include "../model/project.h"
#include "../config/simulation.h"
#include "../appprovider.h"
#include "../logforwarder.h"
#include "../simulationstatuschecker.h"
#include "qfilesystemwatcher.h"
#include "src/command/fileobject.h"

#include <QDebug>
#include <QDir>
#include <cmath>

namespace Command {

RunSimulation::RunSimulation(
  std::shared_ptr<Model::Project> project,
  QObject *parent)
  : Executor(parent)
  , Step(
    {
      { FileObject::Category::Coordinates, { FileObject::Type::GRO } }
    },
    {
      FileObject::Type::GRO,
      FileObject::Type::XTC
    },
    std::make_shared<Config::Simulation>()
    )
  , project(project)
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

  GromacsConfigFileGenerator(simulation).generate(mdpFile);

  QString inputStructure = project->getSystemSetup()->getProcessedStructureFile();
  QFileInfo systemPath(inputStructure);
  //if (simulation->getPreviousStep())
  //{
  //  SimulationStatusChecker prevSimChecker(project, simulation->getPreviousStep());
  //  inputStructure = prevSimChecker.getCoordinatesPath();
  //}

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

  SimulationStatusChecker statusChecker(project, simulation);
  float progressValue = statusChecker.getProgress();
  if (std::isnan(progressValue))
  {
    return;
  }

  emit progress(
    progressValue,
    simulation->isMinimisation() ? ProgressType::Value : ProgressType::Percentage
    );
}

}
