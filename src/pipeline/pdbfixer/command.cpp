#include "command.h"
#include "filenamegenerator.h"

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

  QStringList args;
  args << inputFile;
  args << "--output=" + outputFile;
  args << "--replace-nonstandard";
  args << "--keep-heterogens=none";
  args << "--add-residues";
  args << "--add-atoms=heavy";
  args << "--verbose";

  QFileInfo fileInfo(inputFile);
  QString inputDirectory = fileInfo.absolutePath();
  StatusMessageSetter::getInstance()->setMessage("Executing " + command + " " + args.join(" "));
  process.setWorkingDirectory(inputDirectory);
  process.start(command, args);
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
