#include "creategromacsmodel.h"

#include "../appprovider.h"
#include "../statusmessagesetter.h"
#include "../model/systemsetup.h"

#include <QFileInfo>
#include <QDebug>

namespace Command {

CreateGromacsModel::CreateGromacsModel(
  std::shared_ptr<Model::SystemSetup> newSystemSetup, QObject *parent)
  : Executor(parent)
    , systemSetup(newSystemSetup)
{

}

void CreateGromacsModel::exec()
{
  qDebug() << "Create model";
  QString command = AppProvider::get("gmx");
  if (command.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  command += " pdb2gmx";
  QString inputFile = systemSetup->getFilteredStructureFile();
  QFileInfo fileInfo(inputFile);

  using Model::SystemSetup;
  QString outputFileName = fileInfo.baseName().replace("_filtered", "_processed") + ".gro";
  command += " -f " + inputFile;
  command += " -o " + outputFileName;
  command += " -water " + toString(systemSetup->property("waterModel").value<SystemSetup::WaterModel>());
  command += " -ff " + toString(systemSetup->property("forceField").value<SystemSetup::ForceField>());

  QString inputDirectory = fileInfo.absolutePath();
  process.setWorkingDirectory(inputDirectory);
  process.start(command);

  QString absPathOutputFileName = inputDirectory + "/" + outputFileName;
  systemSetup->setProcessedStructureFile(absPathOutputFileName);
}

}
