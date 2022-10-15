#include "command.h"
#include "configuration.h"
#include "filenamegenerator.h"

#include "../../appprovider.h"
#include "../../statusmessagesetter.h"
#include "../../command/fileobjectconsumer.h"

#include <QFileInfo>
#include <QDebug>

namespace Pipeline { namespace Solvate {

void Command::doExecute()
{
  QString command = AppProvider::get("gmx");
  if (command.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  QStringList args("solvate");
  QString inputFile = getInputFilename();

  auto config = dynamic_cast<Configuration*>(configuration);
  auto waterModel = config->property("waterModel")
    .value<CreateGromacsModel::Configuration::WaterModel>();

  QString outputFile = fileNameGenerator->getFileNameFor(Type::GRO);
  args << "-cp" << inputFile;
  args << "-o" << outputFile;
  args << "-cs" << getWaterBoxFor(waterModel);

  QString outputTopology = fileNameGenerator->getFileNameFor(Type::TOP);
  QFile(fileObjectConsumer->getFileNameFor(Type::TOP)).copy(outputTopology);
  args << "-p" << outputTopology;

  QFileInfo fileInfo(inputFile);
  QString inputDirectory = fileInfo.absolutePath();
  process.setWorkingDirectory(inputDirectory);
  StatusMessageSetter::getInstance()->setMessage("Executing command " + command + " " + args.join(" "));
  process.start(command, args);
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

QString Command::getWaterBoxFor(
  const Pipeline::CreateGromacsModel::Configuration::WaterModel& solvent)
{
  using WaterModel = Pipeline::CreateGromacsModel::Configuration::WaterModel;
  QString waterBox = "spc216.gro";
  if (solvent == WaterModel::TIP4P)
  {
    waterBox = "tip4p.gro";
  }
  else if (solvent == WaterModel::TIP5P)
  {
    waterBox = "tip5p.gro";
  }

  return waterBox;
}

} }
