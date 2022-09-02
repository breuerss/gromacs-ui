#include "command.h"
#include "configuration.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"
#include "../../model/project.h"
#include "../../pdbdownloader.h"

#include <QDebug>
#include <memory>
#include <qobject.h>

namespace Pipeline { namespace PdbDownload {

Command::Command()
  : Executor()
  , downloader(std::make_shared<PdbDownloader>())
{
  auto callback = [this] () {
    qDebug() << __PRETTY_FUNCTION__;
    setRunning(false);
    finished();
  };
  connect(downloader.get(), &PdbDownloader::error, callback);
  connect(downloader.get(), &PdbDownloader::notFound, callback);
  connect(downloader.get(), &PdbDownloader::downloaded, callback);

  connect(this, &Executor::configChanged, [this] (auto configuration) {
    auto newConfig = std::dynamic_pointer_cast<Configuration>(configuration);
    disconnect(configConnection);
    configConnection = connect(
      newConfig.get(), &Configuration::pdbCodeChanged,
      [this] () {
        qDebug() << "pdbcode changed";
        canExecuteChanged(canExecute());
      });
  });
}

void Command::doExecute()
{
  qDebug() << "Exec simulation";

  QString pdbCode = getPdbCode();

  QString fileName = fileNameGenerator->getFileNameFor(::Command::FileObject::Type::PDB);

  setRunning(true);
  downloader->download(pdbCode, fileName);
}

bool Command::canExecute() const
{
  QString pdbCode = getPdbCode();
  return pdbCode.length() == 4;
}

QString Command::getPdbCode() const
{
  return dynamic_cast<Configuration*>(configuration.get())->property("pdbCode").toString();
}

void Command::setFileNameGenerator(std::shared_ptr<FileNameGenerator> newFileNameGenerator)
{
  fileNameGenerator = newFileNameGenerator;
}

} }
