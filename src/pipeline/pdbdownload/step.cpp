#include "step.h"
#include "configuration.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"

namespace Pipeline {

using Registration = FactoryRegistration<PdbDownload::Step>;
template<>
QString Registration::type = Registration::registerMethod("PdbDownload");
template<>
const QString Registration::name("Download PDB");
template<>
const Step::Category Registration::category = Step::Category::DataProvider;

namespace PdbDownload {

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
