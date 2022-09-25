#include "step.h"
#include "configuration.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"
#include "../stepfactory.h"

namespace Pipeline { namespace CreateBox {

QString Step::type = "CreateBox";
bool Step::registered = StepFactory::registerMethod(Step::type, Step::create);

using FileObject = ::Command::FileObject;
Step::Step(std::shared_ptr<Model::Project> project)
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
    std::make_shared<FileNameGenerator>(project),
    Category::PreProcess
    )
{
}

QString Step::getName() const
{
  return "Create Box";
}

QString Step::getType() const
{
  return type;
}

Step::Pointer
Step::create(std::shared_ptr<Model::Project> project)
{
  return std::make_shared<Step>(project);
}

} }
