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

  QString solvatedStructure = getInputFilename();
  QFileInfo fileInfo(solvatedStructure);

  QString ionsBasePath = fileInfo.absolutePath() + "/ions";
  QString ionsMdpPath = ionsBasePath + ".mdp";
  QFile ionsMdp(ionsMdpPath);
  ionsMdp.open(QFile::WriteOnly);
  ionsMdp.close();

  QString inputDirectory = fileInfo.absolutePath();

  QProcess prepCommand;
  prepCommand.setWorkingDirectory(inputDirectory);
  LogForwarder::getInstance()->listenTo(&prepCommand);
  QString ionsTprPath = ionsBasePath + ".tpr";
  QStringList createIonsTprs("grompp");
  createIonsTprs << "-f" << ionsMdpPath;
  createIonsTprs << "-c" << solvatedStructure;
  createIonsTprs << "-p" << "topol.top";
  createIonsTprs << "-o" << ionsTprPath;

  StatusMessageSetter::getInstance()->setMessage("Executing command " + command + " " + createIonsTprs.join(" "));

  prepCommand.start(command, createIonsTprs);
  prepCommand.waitForFinished();
  if (prepCommand.exitCode() != 0)
  {
    qDebug() << prepCommand.exitCode();
    StatusMessageSetter::getInstance()->setMessage("Failed executing " + command + " " + createIonsTprs.join(" "));
  }
  LogForwarder::getInstance()->detach(&prepCommand);

  QStringList args("genion");

  QString outputFile = fileNameGenerator
    ->getFileNameFor(::Command::FileObject::Type::GRO);
  args << "-s" << ionsTprPath;
  args << "-o" << outputFile;
  auto config = dynamic_cast<Configuration*>(configuration);
  args << "-conc" << QString::number(config->property("ionConcentration").value<double>());
  args << "-p" << "topol.top";

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

  process.setWorkingDirectory(inputDirectory);
  StatusMessageSetter::getInstance()->setMessage("Executing command " + command + " " + args.join(" "));

  process.start(command, args);
  process.write("SOL");
  process.closeWriteChannel();
}

bool Command::canExecute() const
{
  return QFile(getInputFilename()).exists();
}

QString Command::getInputFilename() const
{
  using Type = ::Command::FileObject::Type;
  return fileObjectConsumer->getFileNameFor(Type::GRO);
}

} }
