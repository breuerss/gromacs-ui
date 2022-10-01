#include "step.h"
#include "configuration.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"

namespace Pipeline {

template<>
QString FactoryRegistration<CreateBox::Step>::type =
  FactoryRegistration<CreateBox::Step>::registerMethod("CreateBox");

namespace CreateBox {

using FileObject = ::Command::FileObject;
using InputOutput = ::Command::InputOutput;

Step::Step(std::shared_ptr<Model::Project> project)
  : FactoryRegistration(
    project,
    {
      { InputOutput::Category::Coordinates,
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
} }
