#include "command.h"
#include "configuration.h"
#include "filenamegenerator.h"
#include "../../gromacsconfigfilegenerator.h"
#include "../../statusmessagesetter.h"
#include "../../model/project.h"
#include "../../command/fileobjectconsumer.h"
#include "../../appprovider.h"
#include "../../logforwarder.h"
#include "../../simulationstatuschecker.h"
#include "src/command/fileobject.h"
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
    progressChecker.removePath(
      fileNameGenerator->getFileNameFor(::Command::FileObject::Type::LOG)
      );
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
  QString command = AppProvider::get("gmx");
  if (command.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  auto simulationConfig = dynamic_cast<const Pipeline::Simulation::Configuration*>(configuration);
  simulationChecker = std::make_shared<SimulationStatusChecker>(project, simulationConfig);
  QString mdpFile = fileNameGenerator
    ->getFileNameFor(::Command::FileObject::Type::MDP);
  QFileInfo fi(mdpFile);
  QDir dir(fi.absolutePath());
  dir.mkpath(".");

  GromacsConfigFileGenerator(const_cast<Pipeline::Simulation::Configuration*>(simulationConfig))
    .generate(mdpFile);

  using Type = ::Command::FileObject::Type;
  QString inputStructure = getInputFileName();
  QString topology = fileObjectConsumer->getFileNameFor(Type::TOP);
  QFileInfo systemPath(inputStructure);

  if (!execGrompp(
      mdpFile,
      inputStructure,
      topology,
      fileNameGenerator->getFileNameFor(Type::TPR),
      dir.absolutePath()
      ))
  {
    return;
  }

  QStringList args("mdrun");
  args << "-deffnm" << simulationConfig->getTypeAsString();
  args << "-cpt" << "5";
  args << "-cpnum";
  if (simulationConfig->property("resume").value<bool>())
  {
    args += "-cpi";
  }

  StatusMessageSetter::getInstance()->setMessage("Executing " + command + " " + args.join(" "));
  process.setWorkingDirectory(dir.absolutePath());
  process.start(command, args);

  checkProgress();
}

QString Command::getInputFileName() const
{
  using Type = ::Command::FileObject::Type;
  return fileObjectConsumer->getFileNameFor(Type::GRO);
}

bool Command::canExecute() const
{
  QString simulationType = dynamic_cast<const Configuration*>(configuration)
    ->getTypeAsString();
  using Type = ::Command::FileObject::Type;
  auto topology =  fileObjectConsumer->getFileNameFor(Type::TOP);
  return !simulationType.isEmpty() &&
    QFile(getInputFileName()).exists() &&
    QFile(topology).exists();
}

bool Command::execGrompp(
  const QString& mdpFile,
  const QString& inputStructure,
  const QString& topology,
  const QString& output,
  const QString& workingDirectory
  )
{
  QString command = AppProvider::get("gmx");

  QStringList args("grompp");
  args << "-f" << mdpFile;
  args << "-c" << inputStructure;
  args << "-p" << topology;
  args << "-maxwarn" << "2";
  args << "-o" << output;

  qDebug() << "executing grompp" << command << args.join(" ");
  QProcess grompp;
  LogForwarder::getInstance()->listenTo(&grompp);
  grompp.setWorkingDirectory(workingDirectory);

  grompp.start(command, args);
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
  QString logPath = fileNameGenerator->getFileNameFor(
    ::Command::FileObject::Type::LOG
    );

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
