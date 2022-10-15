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
const Registration::Config Registration::config = Registration::registerMethod({
  "Solvate", "Solvate", Step::Category::PreProcess
});

namespace Solvate {

using FileObject = ::Command::FileObject;
using Type = FileObject::Type;
using InputOutput = ::Command::InputOutput;
Step::Step(std::shared_ptr<Model::Project> project)
  : FactoryRegistration(
    project,
    {
      { InputOutput::Category::Coordinates, { Type::GRO } },
      { InputOutput::Category::Topology, { Type::TOP } }
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
