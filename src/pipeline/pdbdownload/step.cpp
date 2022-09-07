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
}

QString Step::getName() const
{
  return "PDB Downloader";
}

QString Step::getType() const
{
  return "PdbDownload";
}

Step::Pointer
Step::create(std::shared_ptr<Model::Project> project) const
{
  return std::make_unique<Step>(project);
}

} }
