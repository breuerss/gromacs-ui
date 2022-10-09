#include "step.h"
#include "command.h"
#include "configuration.h"
#include "filenamegenerator.h"
#include "../../model/project.h"
#include <memory>

namespace Pipeline {

using Registration = FactoryRegistration<Simulation::Step>;
template<>
const Registration::Config Registration::config = Registration::registerMethod({
  "Simulation", "Simulation", Step::Category::Simulation
});

namespace Simulation {

using FileObject = ::Command::FileObject;
using InputOutput = ::Command::InputOutput;
using Type = FileObject::Type;

Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : FactoryRegistration(
    project,
    {
      { InputOutput::Category::Coordinates, { Type::GRO } },
      { InputOutput::Category::Topology, { Type::TOP } },
    },
    {
      Type::GRO,
      Type::XTC,
      Type::LOG,
      Type::MDP,
      Type::TPR,
      Type::EDR,
    },
    std::make_shared<Configuration>(),
    std::make_shared<Command>(project),
    std::make_shared<FileNameGenerator>(project)
  )
{
}

QString Step::getName() const
{
  return config.name + " | " + std::dynamic_pointer_cast<Configuration>(configuration)
    ->getTypeAsString().toUpper();
}

} }
