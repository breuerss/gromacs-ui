#include "step.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../model/project.h"
#include "../stepfactory.h"

namespace Pipeline { namespace SmoothTrajectory {

QString Step::type = "SmoothTrajectory";
bool Step::registered = StepFactory::registerMethod(Step::type, Step::create);

using FileObject = ::Command::FileObject;

Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : Pipeline::Step(
    project,
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
  return "Smooth Trajectory";
}

QString Step::getType() const
{
  return type;
}

Step::Pointer
Step::create(std::shared_ptr<Model::Project> project)
{
  return std::make_unique<Step>(project);
}

} }
