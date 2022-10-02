#include "step.h"
#include "configuration.h"
#include "command.h"
#include "filenamegenerator.h"
#include <memory>
#include "../../command/fileobject.h"
#include "../stepfactory.h"

namespace Pipeline {

using Registration = FactoryRegistration<Solvate::Step>;
template<>
QString Registration::type = Registration::registerMethod("Solvate");
template<>
const QString Registration::name("Solvate");
template<>
const Step::Category Registration::category = Step::Category::PreProcess;

namespace Solvate {

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
