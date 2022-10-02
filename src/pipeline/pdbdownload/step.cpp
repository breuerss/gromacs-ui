#include "step.h"
#include "configuration.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"

namespace Pipeline {

template<>
QString FactoryRegistration<PdbDownload::Step>::type =
  FactoryRegistration<PdbDownload::Step>::registerMethod("PdbDownload");
template<>
const QString FactoryRegistration<PdbDownload::Step>::name("Download PDB");

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
    std::make_shared<FileNameGenerator>(project),
    Category::DataProvider
    )
{
}

} }
