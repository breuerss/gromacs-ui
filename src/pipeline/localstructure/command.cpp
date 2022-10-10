#include "command.h"
#include "configuration.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"
#include "../../model/project.h"

#include <QDebug>
#include <memory>
#include <QFileInfo>
#include <QDir>

namespace Pipeline { namespace LocalStructure {

Command::Command()
  : Executor()
{
  connect(this, &Executor::configChanged, [this] (auto configuration) {
    auto newConfig = dynamic_cast<const Configuration*>(configuration);
    disconnect(configConnection);
    configConnection = connect(
      newConfig, &Configuration::fileNameChanged,
      [this] () {
        canExecuteChanged(canExecute());
      });
  });
}

void Command::doExecute()
{
  qDebug() << "Copy source structure";

  QString inputFilename = getFileName();

  QString outputFileName = fileNameGenerator
    ->getFileNameFor(::Command::FileObject::Type::PDB);

  setRunning(true);

  QFileInfo fileInfo(outputFileName);
  if (fileInfo.exists())
  {
    QFile outFile(outputFileName);
    QString backFileName;
    unsigned index = 0;
    do
    {
      backFileName = outputFileName + ".bak_" + QString::number(index);
      index++;
    }
    while (!outFile.rename(backFileName));
    qDebug() << "Created backup of" << outputFileName << "to" << backFileName;
  }
  else
  {
    // ensure path exists
    QDir dir(fileInfo.absolutePath());
    dir.mkpath(".");
  }

  qDebug() << "Copying" << inputFilename << "to" << outputFileName;
  QFile(inputFilename).copy(outputFileName);
  setRunning(false);
  finished(true);
}

bool Command::canExecute() const
{
  return QFile(getFileName()).exists();
}

QString Command::getFileName() const
{
  return dynamic_cast<const Configuration*>(configuration)
    ->property("fileName").toString();
}

} }
