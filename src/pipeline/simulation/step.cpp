#include "step.h"
#include "command.h"
#include "configuration.h"
#include "filenamegenerator.h"
#include "../../model/project.h"

namespace Pipeline { namespace Simulation {

using FileObject = ::Command::FileObject;

Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : Pipeline::Step(
    {
      { FileObject::Category::Coordinates, { FileObject::Type::GRO } },
      { FileObject::Category::Topology, { FileObject::Type::TOP } },
    },
    {
      FileObject::Type::GRO,
      FileObject::Type::XTC,
      FileObject::Type::LOG,
      FileObject::Type::MDP,
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
