#include "step.h"
#include "configuration.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"

namespace Pipeline {

using Registration = FactoryRegistration<LocalStructure::Step>;
template<>
const Registration::Config Registration::config = Registration::registerMethod({
  "LocalStructure", "PDB from file", Step::Category::DataProvider
});

namespace LocalStructure {

Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : FactoryRegistration(
    project,
    { },
    {
      ::Command::FileObject::Type::PDB,
    },
    std::make_shared<Configuration>(),
    std::make_shared<Command>(),
    std::make_shared<FileNameGenerator>(project)
    )
{
}

} }
