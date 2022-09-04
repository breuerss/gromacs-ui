#include "command.h"

#include "../../appprovider.h"
#include "../../statusmessagesetter.h"
#include "../../command/fileobjectconsumer.h"
#include "configuration.h"
#include "../creategromacsmodel/configuration.h"

#include <QFileInfo>
#include <QDebug>

namespace Pipeline { namespace Solvate {

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

  command += " solvate";
  QString inputFile = getInputFilename();

  auto config = dynamic_cast<Configuration*>(configuration);
  auto waterModel = config->property("waterModel")
    .value<CreateGromacsModel::Configuration::WaterModel>();

  QString outputFile = getOutputFilename();
  command += " -cp " + inputFile;
  command += " -o " + outputFile;
  command += " -cs " + getWaterBoxFor(waterModel);
  command += " -p topol.top";

  QFileInfo fileInfo(inputFile);
  QString inputDirectory = fileInfo.absolutePath();
  process.setWorkingDirectory(inputDirectory);
  StatusMessageSetter::getInstance()->setMessage("Executing command " + command);
  process.start(command);
}

bool Command::canExecute() const
{
  return QFile(getInputFilename()).exists();
}

QString Command::getName() const
{
  return "Solvation";
}

QString Command::getInputFilename() const
{
  using Type = ::Command::FileObject::Type;
  return fileObjectConsumer->getFileNameFor(Type::GRO);
}

QString Command::getOutputFilename() const
{
  QFileInfo fileInfo(getInputFilename());
  return fileInfo.absolutePath() + "/" +
    fileInfo.baseName() + "_solvated.gro";
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
