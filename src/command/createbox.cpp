#include "createbox.h"

#include "../appprovider.h"
#include "../statusmessagesetter.h"
#include "../model/systemsetup.h"
#include <QFileInfo>
#include <QDebug>

namespace Command {

CreateBox::CreateBox(std::shared_ptr<Model::SystemSetup> newSystemSetup, QObject *parent)
  : Executor(parent)
    , systemSetup(newSystemSetup)
{

}

void CreateBox::doExecute()
{
  qDebug() << getName();
  QString command = AppProvider::get("gmx");
  if (command.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  command += " editconf";
  QString inputFile = getInputFilename();

  QString outputFile = getOutputFilename();
  command += " -f " + inputFile;
  command += " -o " + outputFile;
  command += " -d " + systemSetup->property("distance").value<QString>();
  command += " -bt " + toString(systemSetup->property("boxType").value<Model::SystemSetup::BoxType>());

  QFileInfo fileInfo(inputFile);
  QString inputDirectory = fileInfo.absolutePath();
  StatusMessageSetter::getInstance()->setMessage("Executing " + command);
  process.setWorkingDirectory(inputDirectory);
  process.start(command);
}

QString CreateBox::getName() const
{
  return "Box Creation";
}

QString CreateBox::getOutputFilename() const
{
  QFileInfo fileInfo(getInputFilename());

  return fileInfo.absolutePath() + "/" + fileInfo.baseName() + "_boxed.gro";
}

}
