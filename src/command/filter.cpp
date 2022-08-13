#include "filter.h"

#include "../appprovider.h"
#include "../statusmessagesetter.h"
#include "../logforwarder.h"
#include "../model/systemsetup.h"
#include <QProcess>
#include <QFileInfo>
#include <QTranslator>
#include <QDebug>
#include <QTemporaryFile>

namespace Command {

Filter::Filter(std::shared_ptr<Model::SystemSetup> systemSetup, QObject *parent)
  : Executor(parent)
  , systemSetup(systemSetup)
{
}

void Filter::doExecute()
{
  qDebug() << getName();
  QString fileName = getInputFilename();
  QString outputFileName = getOutputFilename();

  QString command = AppProvider::get("pdb_selchain");
  if (command.isEmpty())
  {
    StatusMessageSetter::getInstance()->setMessage("Path to pdb_selchain is not set.");
    return;
  }
  QStringList arguments;
  arguments << "-" + systemSetup->getChains().join(",") << fileName;

  qDebug() << command << arguments;
  LogForwarder::getInstance()->listenTo(&process);
  process.setStandardOutputFile(outputFileName);
  process.start(command, arguments);
  process.waitForFinished();

  // TODO split into different command/remove altogether if pdbfixer is present
  if (systemSetup->property("removeHeteroAtoms").value<bool>())
  {
    QString tmpFileName;
    {
      QTemporaryFile tmp;
      tmp.open();
      tmpFileName = tmp.fileName();
    }
    QFile(outputFileName).copy(tmpFileName);

    command = AppProvider::get("pdb_delhetatm");
    if (command.isEmpty())
    {
      StatusMessageSetter::getInstance()->setMessage("Path to pdb_delhetatm is not set.");
      return;
    }

    command += " " + tmpFileName;
    qDebug() << command;
    //process.start(command);
  }
}

QString Filter::getName() const
{
  return "PDB filter";
}

QString Filter::getOutputFilename() const
{
  QFileInfo fileInfo(getInputFilename());
  return fileInfo.absolutePath() + "/" +
    fileInfo.baseName() + "_filtered.pdb";
}

}
