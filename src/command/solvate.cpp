#include "solvate.h"

#include "../settings.h"
#include "../statusmessagesetter.h"
#include "../model/systemsetup.h"

#include <QFileInfo>
#include <QDebug>

namespace Command {

Solvate::Solvate(std::shared_ptr<Model::SystemSetup> newSystemSetup, QObject *parent)
  : Executor{parent}
, systemSetup(newSystemSetup)
{

}

void Solvate::exec()
{
  qDebug() << "Solvate";
  Settings settings;
  QString command = settings.value(Settings::GMX_PATH).toString();
  if (command.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  command += " solvate";
  QString inputFile = systemSetup->getBoxedStructureFile();
  QFileInfo fileInfo(inputFile);

  QString outputFile = fileInfo.baseName().replace("_boxed", "_solvated") + ".gro";
  command += " -cp " + inputFile;
  command += " -o " + outputFile;
  command += " -cs " + getWaterBoxFor(systemSetup->property("waterModel").value<QString>());
  command += " -p topol.top";

  QString inputDirectory = fileInfo.absolutePath();
  process.setWorkingDirectory(inputDirectory);
  StatusMessageSetter::getInstance()->setMessage("Executing command " + command);
  process.start(command);
  systemSetup->setSolvatedStructureFile(inputDirectory + "/" + outputFile);
}

QString Solvate::getWaterBoxFor(const QString &solvent)
{
  QString waterBox = "spc216.gro";
  if (solvent == "tip4p")
  {
    waterBox = "tip4p.gro";
  }
  else if (solvent == "tip5p")
  {
    waterBox = "tip5p.gro";
  }

  return waterBox;
}

}
