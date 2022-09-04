#include "step.h"
#include "configuration.h"
#include "command.h"
#include "../../command/fileobject.h"

namespace Pipeline { namespace CreateGromacsModel {

using FileObject = ::Command::FileObject;
Step::Step(
    std::shared_ptr<Model::Project>
  )
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
  return "Create GROMACS Model";
}

} }
