#include "command.h"

#include "../../appprovider.h"
#include "../../statusmessagesetter.h"
#include "../../command/fileobjectconsumer.h"
#include "../timestepcontrol/configuration.h"
#include "filenamegenerator.h"
#include <QFileInfo>

namespace Pipeline { namespace Temperature {

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

  QStringList args("energy");
  args << "-f" << getInputFilename();
  args << "-o" << outputFile;

  StatusMessageSetter::getInstance()->setMessage("Executing " + command + args.join(" "));
  process.start(command, args);
  process.write("Temperature");
  process.closeWriteChannel();
}

QString Command::getInputFilename() const
{
  return fileObjectConsumer->getFileNameFor(Type::EDR);
}

bool Command::canExecute() const
{
  return QFile(getInputFilename()).exists();
}

} }
