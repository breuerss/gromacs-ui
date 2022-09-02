#include "step.h"
#include "configuration.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"

namespace Pipeline { namespace PdbDownload {

Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : Pipeline::Step(
    { },
    {
      ::Command::FileObject::Type::PDB,
    },
    std::make_shared<Configuration>(),
    std::make_shared<Command>(),
    std::make_shared<FileNameGenerator>(project),
    Category::DataProvider
    )
{
  std::dynamic_pointer_cast<Command>(command)
    ->setFileNameGenerator(std::dynamic_pointer_cast<FileNameGenerator>(fileNameGenerator));
  fileNameGenerator->setConfiguration(getConfiguration());
}

QString Step::getName() const
{
  return "PDB Downloader";
}

} }
