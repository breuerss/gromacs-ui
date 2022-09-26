#include "step.h"
#include "command.h"
#include "configuration.h"
#include "filenamegenerator.h"
#include "../../model/project.h"

namespace Pipeline {

template<>
bool FactoryRegistration<Simulation::Step>::registered =
  FactoryRegistration<Simulation::Step>::registerMethod("Simulation");

namespace Simulation {

using FileObject = ::Command::FileObject;
using Type = FileObject::Type;

Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : FactoryRegistration(
    project,
    {
      { FileObject::Category::Coordinates, { Type::GRO } },
      { FileObject::Category::Topology, { Type::TOP } },
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
  return "Simulation";
}

} }
