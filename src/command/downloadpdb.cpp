#include "downloadpdb.h"
#include "../statusmessagesetter.h"
#include "../model/systemsetup.h"
#include "../model/project.h"
#include "../config/simulation.h"
#include "../appprovider.h"
#include "../logforwarder.h"
#include "../simulationstatuschecker.h"
#include "qfilesystemwatcher.h"
#include "src/command/fileobject.h"
#include "src/pdbdownloader.h"

#include <QDebug>
#include <QString>
#include <QDir>
#include <cmath>
#include <memory>

namespace Command {

DownloadPdb::DownloadPdb(
  std::shared_ptr<Model::Project> project,
  QObject *parent)
  : Executor(parent)
  , Step(
    { },
    {
      FileObject::Type::PDB,
    },
    std::make_shared<Config::Pdb>(),
    Category::DataProvider
    )
  , project(project)
  , downloader(std::make_shared<PdbDownloader>())
{
}

void DownloadPdb::doExecute()
{
  qDebug() << "Exec simulation";

  auto pdbCode = std::get<std::shared_ptr<Config::Pdb>>(configuration)->property("pdbCode").toString();

  QString fileName = project->getProjectPath();
  fileName += QString("/input/%1.pdb").arg(pdbCode);

  downloader->download(pdbCode, fileName);
}

QString DownloadPdb::getName() const
{
  return "PDB Downloader";
}

}
