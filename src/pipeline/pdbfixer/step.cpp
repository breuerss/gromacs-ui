#include "step.h"
#include "configuration.h"
#include "command.h"
#include "../../command/fileobject.h"

namespace Pipeline { namespace PdbFixer {

using FileObject = ::Command::FileObject;
Step::Step(
    std::shared_ptr<Model::Project>
  )
  : Pipeline::Step(
    {
      { FileObject::Category::Coordinates, { FileObject::Type::PDB } }
    },
    {
      FileObject::Type::PDB,
    },
    std::make_shared<Configuration>(),
    std::make_shared<Command>(),
    nullptr,
    Category::Preprocess
    )
{
}

QString Step::getName() const
{
  return "PDB Fixer";
}

QString Step::getType() const
{
  return "PdbFixer";
}

Step::Pointer
Step::create(std::shared_ptr<Model::Project> project) const
{
  return std::make_unique<Step>(project);
}


} }
