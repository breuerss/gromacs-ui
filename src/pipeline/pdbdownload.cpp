#include "pdbdownload.h"
#include "../command/fileobject.h"
#include "../config/pdb.h"
#include "../command/downloadpdb.h"
#include "../command/inputfilenamegenerator.h"
#include <memory>

namespace Pipeline {

PdbDownload::PdbDownload(
    std::shared_ptr<Model::Project> project
  )
  : Step(
    { },
    {
      Command::FileObject::Type::PDB,
    },
    std::make_shared<Config::Pdb>(),
    std::make_shared<Command::DownloadPdb>(),
    std::make_shared<Command::InputFileNameGenerator>(project),
    Category::DataProvider
    )
{
  std::dynamic_pointer_cast<Command::DownloadPdb>(command)
    ->setFileNameGenerator(std::dynamic_pointer_cast<Command::InputFileNameGenerator>(fileNameGenerator));
  fileNameGenerator->setConfiguration(getConfiguration());
}

QString PdbDownload::getName() const
{
  return "PDB Downloader";
}

}
