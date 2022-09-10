#include "command.h"

#include "../../appprovider.h"
#include "../../statusmessagesetter.h"
#include "../../command/fileobjectconsumer.h"
#include "../../command/fileobject.h"
#include "configuration.h"

#include <QFileInfo>
#include <QDebug>
#include <stdexcept>

namespace Pipeline { namespace CreateGromacsModel {

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

  command += " pdb2gmx";
  QString inputFile = getInputFilename();

  const QString outputFileName =
    fileNameGenerator->getFileNameFor(::Command::FileObject::Type::GRO);
  command += " -f " + inputFile;
  command += " -o " + outputFileName;
  command += " -ignh ";

  auto config = dynamic_cast<Configuration*>(configuration);
  command += " -water " + toString(config->property("waterModel")
                                   .value<Configuration::WaterModel>());
  command += " -ff " + toString(config->property("forceField")
                                .value<Configuration::ForceField>());

  QFileInfo fileInfo(inputFile);
  QString inputDirectory = fileInfo.absolutePath();
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

QString Command::getName() const
{
  return "GROMACS model creation";
}

} }
