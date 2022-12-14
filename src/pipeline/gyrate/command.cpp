#include "command.h"

#include "../../appprovider.h"
#include "../../statusmessagesetter.h"
#include "../../command/fileobjectconsumer.h"
#include "filenamegenerator.h"
#include <QFileInfo>

namespace Pipeline { namespace Gyrate {

void Command::doExecute()
{
  QString command = AppProvider::get("gmx");
  if (command.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  QString outputFile = fileNameGenerator
    ->getFileNameFor(Type::XVG);

  QStringList args("gyrate");
  args << "-f" << getInputFilename();
  args << "-s" << fileObjectConsumer->getFileNameFor(Type::TPR);
  args << "-o" << outputFile;

  QString inputFile = getInputFilename();
  QFileInfo fileInfo(inputFile);
  QString inputDirectory = fileInfo.absolutePath();
  StatusMessageSetter::getInstance()->setMessage("Executing " + command + args.join(" "));
  process.setWorkingDirectory(inputDirectory);
  process.start(command, args);
  process.write("Protein");
  process.closeWriteChannel();
}

QString Command::getInputFilename() const
{
  return fileObjectConsumer->getFileNameFor(Type::XTC);
}

bool Command::canExecute() const
{
  return QFile(getInputFilename()).exists() &&
    QFile(fileObjectConsumer->getFileNameFor(Type::TPR)).exists();
}

} }
