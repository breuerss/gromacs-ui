#include "step.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../model/project.h"

namespace Pipeline { namespace CenterProtein {

using FileObject = ::Command::FileObject;

Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : Pipeline::Step(
    {
      { FileObject::Category::Coordinates, { FileObject::Type::GRO } },
      { FileObject::Category::Trajectory, { FileObject::Type::XTC } },
    },
    {
      FileObject::Type::XTC
    },
    nullptr,
    std::make_shared<Command>(),
    std::make_shared<FileNameGenerator>(project),
    Category::PostProcess
    )
{
}

QString Step::getName() const
{
  return "Center Protein";
}

QString Step::getType() const
{
  return "CenterProtein";
}

Step::Pointer
Step::create(std::shared_ptr<Model::Project> project) const
{
  return std::make_unique<Step>(project);
}

} }
