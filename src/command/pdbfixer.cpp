#include "pdbfixer.h"

#include "../appprovider.h"
#include "../statusmessagesetter.h"
#include "../model/systemsetup.h"
#include "qfileinfo.h"
#include <QFileInfo>
#include <QDebug>

namespace Command {

PdbFixer::PdbFixer()
  : Executor()
{

}

void PdbFixer::doExecute()
{
  qDebug() << getName();
  QString command = AppProvider::get("pdbfixer");
  if (command.isEmpty())
  {
    QString message("Path to 'pdbfixer' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  QString inputFile = getInputFilename();

  QString outputFile = getOutputFilename();
  command += " " + inputFile;
  command += " --output=" + outputFile;
  command += " --replace-nonstandard";
  command += " --keep-heterogens=none";
  command += " --add-residues";
  command += " --add-atoms=heavy";
  command += " --verbose";

  QFileInfo fileInfo(inputFile);
  QString inputDirectory = fileInfo.absolutePath();
  StatusMessageSetter::getInstance()->setMessage("Executing " + command);
  process.setWorkingDirectory(inputDirectory);
  process.start(command);
}

bool PdbFixer::canExecute() const
{
  return QFile(getInputFilename()).exists();
}

QString PdbFixer::getName() const
{
  return "pdbfixer";
}

QString PdbFixer::getOutputFilename() const
{
  QFileInfo fileInfo(getInputFilename());

  return fileInfo.absolutePath() + "/" + fileInfo.baseName() + "_fixed.pdb";
}

}
