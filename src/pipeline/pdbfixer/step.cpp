#include "step.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"

namespace Pipeline {

using Registration = FactoryRegistration<PdbFixer::Step>;
template<>
QString Registration::type = Registration::registerMethod("PdbFixer");
template<>
const QString Registration::name("PDB Fixer");
template<>
const Step::Category Registration::category = Step::Category::PreProcess;

namespace PdbFixer {

using FileObject = ::Command::FileObject;
using InputOutput = ::Command::InputOutput;
Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : FactoryRegistration(
    project,
    {
      { InputOutput::Category::Coordinates, { FileObject::Type::PDB } }
    },
    {
      FileObject::Type::PDB,
    },
    nullptr,
    std::make_shared<Command>(),
    std::make_shared<FileNameGenerator>(project)
    )
{
}

} }
