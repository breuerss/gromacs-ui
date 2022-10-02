#include "step.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../model/project.h"

namespace Pipeline {

template<>
QString FactoryRegistration<SmoothTrajectory::Step>::type =
  FactoryRegistration<SmoothTrajectory::Step>::registerMethod("SmoothTrajectory");
template<>
const QString FactoryRegistration<SmoothTrajectory::Step>::name("Smooth Trajectory");

namespace SmoothTrajectory {

using FileObject = ::Command::FileObject;
using InputOutput = ::Command::InputOutput;

Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : FactoryRegistration(
    project,
    {
      { InputOutput::Category::Coordinates, { FileObject::Type::GRO } },
      { InputOutput::Category::Trajectory, { FileObject::Type::XTC } },
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

} }
