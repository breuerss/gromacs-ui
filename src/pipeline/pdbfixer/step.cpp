#include "step.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"

namespace Pipeline {

template<>
QString FactoryRegistration<PdbFixer::Step>::type =
  FactoryRegistration<PdbFixer::Step>::registerMethod("PdbFixer");
template<>
const QString FactoryRegistration<PdbFixer::Step>::name("PDB Fixer");

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
    std::make_shared<FileNameGenerator>(project),
    Category::PreProcess
    )
{
}

} }
