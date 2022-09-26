#include "step.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../model/project.h"

namespace Pipeline {

template<>
bool FactoryRegistration<SmoothTrajectory::Step>::registered =
  FactoryRegistration<SmoothTrajectory::Step>::registerMethod("SmoothTrajectory");

namespace SmoothTrajectory {

using FileObject = ::Command::FileObject;

Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : FactoryRegistration(
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

} }
