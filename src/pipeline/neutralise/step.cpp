#include "step.h"
#include "configuration.h"
#include "command.h"
#include "filenamegenerator.h"

namespace Pipeline {

using Registration = FactoryRegistration<Neutralise::Step>;
template<>
const Registration::Config Registration::config = Registration::registerMethod({
  "Neutralise", "Neutralise", Step::Category::PreProcess
});

namespace Neutralise {

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
