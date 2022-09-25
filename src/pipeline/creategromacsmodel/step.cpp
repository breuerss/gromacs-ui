#include "step.h"
#include "configuration.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"
#include "../stepfactory.h"

namespace Pipeline { namespace CreateGromacsModel {

QString Step::type = "CreateGromacsModel";
bool Step::registered = StepFactory::registerMethod(Step::type, Step::create);

using FileObject = ::Command::FileObject;
Step::Step(std::shared_ptr<Model::Project> project)
  : Pipeline::Step(
    {
      { FileObject::Category::Coordinates,
        {
          FileObject::Type::PDB, FileObject::Type::GRO
        }
      }
    },
    {
      FileObject::Type::GRO,
      FileObject::Type::TOP,
    },
    std::make_shared<Configuration>(),
    std::make_shared<Command>(),
    std::make_shared<FileNameGenerator>(project),
    Category::PreProcess
    )
{
}

QString Step::getName() const
{
  return "Create GROMACS Model";
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
