#include "step.h"
#include "configuration.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"
#include "../stepfactory.h"

namespace Pipeline { namespace PdbDownload {

QString Step::type = "PdbDownload";
bool Step::registered = StepFactory::registerMethod(Step::type, Step::create);

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
  return type;
}

Step::Pointer
Step::create(std::shared_ptr<Model::Project> project)
{
  return std::make_unique<Step>(project);
}

} }
