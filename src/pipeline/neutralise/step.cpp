#include "step.h"
#include "configuration.h"
#include "command.h"
#include <memory>
#include "../../command/fileobject.h"

namespace Pipeline { namespace Neutralise {

using FileObject = ::Command::FileObject;
Step::Step(
    std::shared_ptr<Model::Project>
  )
  : Pipeline::Step(
    {
      { FileObject::Category::Coordinates,
        {
          FileObject::Type::GRO
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
  return "Neutralise";
}

QString Step::getType() const
{
  return "Neutralise";
}

Step::Pointer
Step::create(std::shared_ptr<Model::Project> project) const
{
  return std::make_unique<Step>(project);
}

} }
