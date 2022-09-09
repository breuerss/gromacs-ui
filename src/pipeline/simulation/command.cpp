#include "command.h"
#include "../../gromacsconfigfilegenerator.h"
#include "../../statusmessagesetter.h"
#include "../../model/project.h"
#include "configuration.h"
#include "../../appprovider.h"
#include "../../logforwarder.h"
#include "../../simulationstatuschecker.h"
#include "qfilesystemwatcher.h"
#include "src/command/fileobject.h"
#include "src/command/fileobjectconsumer.h"
#include "ui/simulationstatus.h"

#include <QDebug>
#include <QDir>
#include <cmath>
#include <memory>

namespace Pipeline { namespace Simulation {

Command::Command(std::shared_ptr<Model::Project> project)
  : Executor()
  , project(project)
{
  connect(this, &Command::finished, [this] () {
    progressChecker.removePath(simulationChecker->getLogPath());
  });
  connect(this, &Command::started, [this] () {
    auto simulationConfig = dynamic_cast<const Pipeline::Simulation::Configuration*>(configuration);
    progress(0, simulationConfig->isMinimisation() ? ProgressType::Value : ProgressType::Percentage);
  });
  connect(&progressChecker, &QFileSystemWatcher::fileChanged, this, &Command::checkProgress);
}

void Command::doExecute()
{
  qDebug() << "Exec simulation";
  QString gmx = AppProvider::get("gmx");
  if (gmx.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  auto simulationConfig = dynamic_cast<const Pipeline::Simulation::Configuration*>(configuration);
  simulationChecker = std::make_shared<SimulationStatusChecker>(project, simulationConfig);
  QString mdpFile = simulationChecker->getMdpPath();
  QFileInfo fi(mdpFile);
  QDir dir(fi.absolutePath());
  dir.mkpath(".");

  GromacsConfigFileGenerator(const_cast<Pipeline::Simulation::Configuration*>(simulationConfig))
    .generate(mdpFile);

  QString inputStructure = getInputFileName();
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
  command += " -deffnm " + simulationConfig->getTypeAsString();
  command += " -cpt 5";
  if (simulationConfig->property("resume").value<bool>())
  {
    command += " -cpi";
  }

  StatusMessageSetter::getInstance()->setMessage("Executing " + command);
  process.setWorkingDirectory(dir.absolutePath());
  process.start(command);

  checkProgress();
}

QString Command::getInputFileName() const
{
  using Type = ::Command::FileObject::Type;
  return fileObjectConsumer->getFileNameFor(Type::GRO);
}

bool Command::canExecute() const
{
  return QFile(getInputFileName()).exists();
}

bool Command::execGrompp(
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

void Command::checkProgress()
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

  auto simulationConfig = dynamic_cast<const Pipeline::Simulation::Configuration*>(configuration);
  SimulationStatusChecker statusChecker(project, simulationConfig);
  float progressValue = statusChecker.getProgress();
  if (std::isnan(progressValue))
  {
    return;
  }

  progress(
    progressValue,
    simulationConfig->isMinimisation() ? ProgressType::Value : ProgressType::Percentage
    );
}

QWidget* Command::getStatusUi()
{
  return new SimulationStatus(
    this,
    dynamic_cast<Pipeline::Simulation::Configuration*>(configuration));
}

} }