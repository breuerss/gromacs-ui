#include "command.h"

#include "../../appprovider.h"
#include "../../statusmessagesetter.h"
#include "../../command/fileobjectconsumer.h"
#include "configuration.h"
#include <QFileInfo>
#include <QDebug>

namespace Pipeline { namespace CreateBox {

Command::Command()
  : Executor()
{

}

void Command::doExecute()
{
  qDebug() << getName();
  QString command = AppProvider::get("gmx");
  if (command.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  command += " editconf";
  QString inputFile = getInputFilename();

  QString outputFile = getOutputFilename();
  command += " -f " + inputFile;
  command += " -o " + outputFile;
  auto config = dynamic_cast<Configuration*>(configuration);
  command += " -d " + config->property("distance").value<QString>();
  command += " -bt " + toString(config->property("boxType").value<Configuration::BoxType>());

  QFileInfo fileInfo(inputFile);
  QString inputDirectory = fileInfo.absolutePath();
  StatusMessageSetter::getInstance()->setMessage("Executing " + command);
  process.setWorkingDirectory(inputDirectory);
  process.start(command);
}

bool Command::canExecute() const
{
  return QFile(getInputFilename()).exists();
}

QString Command::getName() const
{
  return "Box Creation";
}

QString Command::getInputFilename() const
{
  using Type = ::Command::FileObject::Type;
  return fileObjectConsumer->getFileNameFor(Type::GRO);
}

QString Command::getOutputFilename() const
{
  QFileInfo fileInfo(getInputFilename());

  return fileInfo.absolutePath() + "/" + fileInfo.baseName() + "_boxed.gro";
}

} }
