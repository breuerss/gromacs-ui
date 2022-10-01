#include "step.h"
#include "command.h"
#include "configuration.h"
#include "filenamegenerator.h"
#include "../../model/project.h"
#include <memory>

namespace Pipeline {

template<>
QString FactoryRegistration<Simulation::Step>::type =
  FactoryRegistration<Simulation::Step>::registerMethod("Simulation");

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
    },
    std::make_shared<Configuration>(),
    std::make_shared<Command>(project),
    std::make_shared<FileNameGenerator>(project),
    Category::Simulation
    )
{
}

QString Step::getName() const
{
  return "Simulation | " + std::dynamic_pointer_cast<Configuration>(configuration)
    ->getTypeAsString().toUpper();
}

} }
