#include "command.h"
#include "configuration.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"
#include "../../model/project.h"
#include "../../pdbdownloader.h"

#include <QDebug>
#include <memory>
#include <QFileInfo>
#include <QDir>

namespace Pipeline { namespace PdbDownload {

Command::Command()
  : Executor()
  , downloader(std::make_shared<PdbDownloader>())
{
  auto callback = [this] () {
    setRunning(false);
    finished(true);
  };
  auto failed = [this] () {
    setRunning(false);
    finished(false);
  };
  connect(downloader.get(), &PdbDownloader::error, failed);
  connect(downloader.get(), &PdbDownloader::notFound, failed);
  connect(downloader.get(), &PdbDownloader::downloaded, callback);

  connect(this, &Executor::configChanged, [this] (auto configuration) {
    auto newConfig = dynamic_cast<const Configuration*>(configuration);
    disconnect(configConnection);
    configConnection = connect(
      newConfig, &Configuration::pdbCodeChanged,
      [this] () {
        canExecuteChanged(canExecute());
      });
  });
}

void Command::doExecute()
{
  QString pdbCode = getPdbCode();

  QString fileName = fileNameGenerator->getFileNameFor(::Command::FileObject::Type::PDB);

  setRunning(true);
  QFileInfo fileInfo(fileName);

  QDir dir(fileInfo.absolutePath());
  dir.mkpath(".");

  downloader->download(pdbCode, fileName);
}

bool Command::canExecute() const
{
  QString pdbCode = getPdbCode();
  return pdbCode.length() == 4;
}

QString Command::getPdbCode() const
{
  return dynamic_cast<const Configuration*>(configuration)->property("pdbCode").toString();
}

} }
