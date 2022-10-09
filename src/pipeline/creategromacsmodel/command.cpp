#include "command.h"

#include "../../appprovider.h"
#include "../../statusmessagesetter.h"
#include "../../command/fileobjectconsumer.h"
#include "../../command/fileobject.h"
#include "configuration.h"
#include "filenamegenerator.h"

#include <QFileInfo>
#include <QDebug>
#include <stdexcept>

namespace Pipeline { namespace CreateGromacsModel {

void Command::doExecute()
{
  QString command = AppProvider::get("gmx");
  if (command.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  QStringList args("pdb2gmx");
  QString inputFile = getInputFilename();

  const QString outputFileName =
    fileNameGenerator->getFileNameFor(::Command::FileObject::Type::GRO);
  args << "-f" << inputFile;
  args << "-o" << outputFileName;
  args << "-ignh";

  auto config = dynamic_cast<Configuration*>(configuration);
  args << "-water" << toString(config->property("waterModel")
                               .value<Configuration::WaterModel>());
  args << "-ff" << toString(config->property("forceField")
                            .value<Configuration::ForceField>());

  QFileInfo fileInfo(inputFile);
  QString inputDirectory = fileInfo.absolutePath();
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
