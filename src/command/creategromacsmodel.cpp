#include "creategromacsmodel.h"

#include "../appprovider.h"
#include "../statusmessagesetter.h"
#include "../model/systemsetup.h"

#include <QFileInfo>
#include <QDebug>
#include <stdexcept>

namespace Command {

CreateGromacsModel::CreateGromacsModel(
  std::shared_ptr<Model::SystemSetup> newSystemSetup, QObject *parent)
  : Executor(parent)
    , systemSetup(newSystemSetup)
{

}

void CreateGromacsModel::doExecute()
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

  const QString outputFileName(getOutputFilename());
  using Model::SystemSetup;
  command += " -f " + inputFile;
  command += " -o " + outputFileName;
  command += " -ignh ";
  command += " -water " + toString(systemSetup->property("waterModel").value<SystemSetup::WaterModel>());
  command += " -ff " + toString(systemSetup->property("forceField").value<SystemSetup::ForceField>());

  QFileInfo fileInfo(inputFile);
  QString inputDirectory = fileInfo.absolutePath();
  process.setWorkingDirectory(inputDirectory);
  process.start(command);
}

QString CreateGromacsModel::getName() const
{
  return "GROMACS model creation";
}

QString CreateGromacsModel::getOutputFilename() const
{
  QFileInfo fileInfo(getInputFilename());
  return fileInfo.absolutePath() + "/" +
    fileInfo.baseName() + "_model.gro";
}

}
