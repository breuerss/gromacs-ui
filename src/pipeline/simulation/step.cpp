#include "step.h"
#include "../../model/project.h"
#include "../../command/fileobject.h"
#include "command.h"
#include "configuration.h"

namespace Pipeline { namespace Simulation {

Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : Pipeline::Step(
    {
      { ::Command::FileObject::Category::Coordinates, { ::Command::FileObject::Type::GRO } }
    },
    {
      ::Command::FileObject::Type::GRO,
      ::Command::FileObject::Type::XTC
    },
    std::make_shared<Configuration>(),
    std::make_shared<Command>(project),
    nullptr, // TODO filename generator for simulation
    Category::Simulation
    )
{
}

QString Step::getName() const
{
  return "Simulation";
}

QString Step::getType() const
{
  return "Simulation";
}

Step::Pointer
Step::create(std::shared_ptr<Model::Project> project) const
{
  return std::make_unique<Step>(project);
}

} }
