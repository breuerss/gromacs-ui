#include "solvate.h"

#include "../appprovider.h"
#include "../statusmessagesetter.h"
#include "../model/systemsetup.h"

#include <QFileInfo>
#include <QDebug>

namespace Command {

Solvate::Solvate(std::shared_ptr<Model::SystemSetup> newSystemSetup)
  : Executor()
    , systemSetup(newSystemSetup)
{

}

void Solvate::doExecute()
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

  using Model::SystemSetup;
  auto waterModel = systemSetup->property("waterModel").value<SystemSetup::WaterModel>();

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

bool Solvate::canExecute() const
{
  return QFile(getInputFilename()).exists();
}

QString Solvate::getName() const
{
  return "Solvation";
}

QString Solvate::getOutputFilename() const
{
  QFileInfo fileInfo(getInputFilename());
  return fileInfo.absolutePath() + "/" +
    fileInfo.baseName() + "_solvated.gro";
}

QString Solvate::getWaterBoxFor(const Model::SystemSetup::WaterModel& solvent)
{
  using Model::SystemSetup;
  QString waterBox = "spc216.gro";
  if (solvent == SystemSetup::WaterModel::TIP4P)
  {
    waterBox = "tip4p.gro";
  }
  else if (solvent == SystemSetup::WaterModel::TIP5P)
  {
    waterBox = "tip5p.gro";
  }

  return waterBox;
}

}
