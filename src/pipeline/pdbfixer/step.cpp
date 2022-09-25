#include "step.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"
#include "../stepfactory.h"

namespace Pipeline { namespace PdbFixer {

QString Step::type = "PdbFixer";
bool Step::registered = StepFactory::registerMethod(Step::type, Step::create);

using FileObject = ::Command::FileObject;
Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : Pipeline::Step(
    {
      { FileObject::Category::Coordinates, { FileObject::Type::PDB } }
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

QString Step::getName() const
{
  return "PDB Fixer";
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
