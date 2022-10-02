#include "step.h"
#include "configuration.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"

namespace Pipeline {

using Registration = FactoryRegistration<CreateBox::Step>;
template<>
QString Registration::type = Registration::registerMethod("CreateBox");
template<>
const QString Registration::name("Create Box");
template<>
const Step::Category Registration::category = Step::Category::PreProcess;

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
    std::make_shared<FileNameGenerator>(project)
    )
{
}

} }
