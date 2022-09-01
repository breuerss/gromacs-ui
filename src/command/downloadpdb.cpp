#include "downloadpdb.h"
#include "inputfilenamegenerator.h"
#include "../model/project.h"
#include "../config/pdb.h"
#include "../pdbdownloader.h"

#include <QDebug>

namespace Command {

DownloadPdb::DownloadPdb()
  : Executor()
  , downloader(std::make_shared<PdbDownloader>())
{
  connect(downloader.get(), &PdbDownloader::downloaded, this, &Executor::finished);
}

void DownloadPdb::doExecute()
{
  qDebug() << "Exec simulation";

  QString pdbCode = getPdbCode();

  QString fileName = fileNameGenerator->getFileNameFor(FileObject::Type::PDB);

  downloader->download(pdbCode, fileName);
}

bool DownloadPdb::canExecute() const
{
  QString pdbCode = getPdbCode();
  return pdbCode.length() == 4;
}

QString DownloadPdb::getPdbCode() const
{
  return std::dynamic_pointer_cast<Config::Pdb>(configuration)->property("pdbCode").toString();
}

void DownloadPdb::setFileNameGenerator(std::shared_ptr<InputFileNameGenerator> newFileNameGenerator)
{
  fileNameGenerator = newFileNameGenerator;
}

}
