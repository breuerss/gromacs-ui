#include "command.h"

#include "../../appprovider.h"
#include "../../statusmessagesetter.h"
#include "../../command/fileobject.h"
#include "../../command/fileobjectconsumer.h"
#include <QFileInfo>

namespace Pipeline { namespace PdbFixer {

void Command::doExecute()
{
  QString command = AppProvider::get("pdbfixer");
  if (command.isEmpty())
  {
    QString message("Path to 'pdbfixer' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  QString inputFile = getInputFilename();

  QString outputFile = fileNameGenerator
    ->getFileNameFor(::Command::FileObject::Type::PDB);
  command += " " + inputFile;
  command += " --output=" + outputFile;
  command += " --replace-nonstandard";
  command += " --keep-heterogens=none";
  command += " --add-residues";
  command += " --add-atoms=heavy";
  command += " --verbose";

  QFileInfo fileInfo(inputFile);
  QString inputDirectory = fileInfo.absolutePath();
  StatusMessageSetter::getInstance()->setMessage("Executing " + command);
  process.setWorkingDirectory(inputDirectory);
  process.start(command);
}

QString Command::getInputFilename() const
{
  using Type = ::Command::FileObject::Type;
  return fileObjectConsumer->getFileNameFor(Type::PDB);
}

bool Command::canExecute() const
{
  return QFile(getInputFilename()).exists();
}

} }
