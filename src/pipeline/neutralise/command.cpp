#include "command.h"

#include "../../appprovider.h"
#include "../../statusmessagesetter.h"
#include "../../logforwarder.h"
#include "../../command/fileobjectconsumer.h"
#include "configuration.h"
#include "filenamegenerator.h"

#include <QDebug>
#include <QFileInfo>

namespace Pipeline { namespace Neutralise {

void Command::doExecute()
{
  QString command = AppProvider::get("gmx");
  if (command.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  setRunning(true);
  auto pair = getPreparationCommand();
  auto prepCommand = pair.first;


  QString ionsTprPath = pair.second;
  connect(
    prepCommand.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
    [this, ionsTprPath, prepCommand] () {
    LogForwarder::getInstance()->detach(prepCommand.get());

    if (prepCommand->exitCode() != 0)
    {
      qDebug() << prepCommand->exitCode();
      StatusMessageSetter::getInstance()->setMessage(
        "Failed executing " + prepCommand->program() + " " + prepCommand->arguments().join(" "));

      return;
    }

    neutralise(ionsTprPath);
  });

}

QPair<std::shared_ptr<QProcess>, QString> Command::getPreparationCommand()
{
  QString command = AppProvider::get("gmx");
  QString solvatedStructure = getInputFilename();
  QFileInfo fileInfo(solvatedStructure);

  QString ionsBasePath = fileInfo.absolutePath() + "/ions";
  QString ionsMdpPath = ionsBasePath + ".mdp";
  QFile ionsMdp(ionsMdpPath);
  ionsMdp.open(QFile::WriteOnly);
  ionsMdp.close();

  QString inputDirectory = fileInfo.absolutePath();

  auto prepCommand = std::make_shared<QProcess>();
  prepCommand->setWorkingDirectory(inputDirectory);
  LogForwarder::getInstance()->listenTo(prepCommand.get());
  QString ionsTprPath = ionsBasePath + ".tpr";
  QStringList createIonsTprs("grompp");
  createIonsTprs << "-f" << ionsMdpPath;
  createIonsTprs << "-c" << solvatedStructure;
  createIonsTprs << "-p" << fileObjectConsumer->getFileNameFor(Type::TOP);
  createIonsTprs << "-o" << ionsTprPath;

  StatusMessageSetter::getInstance()->setMessage("Executing command " + command + " " + createIonsTprs.join(" "));

  prepCommand->start(command, createIonsTprs);

  return { prepCommand, ionsTprPath };
}

void Command::neutralise(const QString& ionsTprPath)
{
  QStringList args("genion");

  QString outputFile = fileNameGenerator
    ->getFileNameFor(::Command::FileObject::Type::GRO);
  args << "-s" << ionsTprPath;
  args << "-o" << outputFile;
  auto config = dynamic_cast<Configuration*>(configuration);
  args << "-conc" << QString::number(config->property("ionConcentration").value<double>());
  args << "-p" << fileObjectConsumer->getFileNameFor(Type::TOP);

  const QString& positiveIon = config->property("positiveIon").value<QString>();
  args << "-pname" << positiveIon;
  static const QMap<QString, int> positiveIonCharge = {
    { "MG", 2 },
    { "CA", 2 },
    { "LI", 1 },
    { "NA", 1 },
    { "K ", 1 },
    { "RB", 1 },
    { "CS", 1 },
  };
  args << "-pq" << QString::number(positiveIonCharge[positiveIon]);
  args << "-nname" << config->property("negativeIon").value<QString>();
  args << "-nq" << "-1";
  args << "-neutral";

  process.setWorkingDirectory(QFileInfo(ionsTprPath).absolutePath());
  StatusMessageSetter::getInstance()->setMessage(
    "Executing command " + process.program() + " " + args.join(" "));

  QString command = AppProvider::get("gmx");
  process.start(command, args);
  process.write("SOL");
  process.closeWriteChannel();
}

bool Command::canExecute() const
{
  return QFile(getInputFilename()).exists() &&
    QFile(fileObjectConsumer->getFileNameFor(Type::TOP)).exists();
}

QString Command::getInputFilename() const
{
  return fileObjectConsumer->getFileNameFor(Type::GRO);
}

} }
