#include "creategromacsmodel.h"

#include "../settings.h"
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
  Settings settings;
  QString command = settings.value(Settings::GMX_PATH).toString();
  if (command.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  command += " pdb2gmx";
  QString inputFile = systemSetup->getFilteredStructureFile();
  QFileInfo fileInfo(inputFile);

  QString outputFileName = fileInfo.baseName().replace("_filtered", "_processed") + ".gro";
  command += " -f " + inputFile;
  command += " -o " + outputFileName;
  command += " -water " + systemSetup->property("waterModel").value<QString>();
  command += " -ff " + systemSetup->property("forceField").value<QString>();

  QString inputDirectory = fileInfo.absolutePath();
  process.setWorkingDirectory(inputDirectory);
  process.start(command);

  QString absPathOutputFileName = inputDirectory + "/" + outputFileName;
  systemSetup->setProcessedStructureFile(absPathOutputFileName);
}

}
