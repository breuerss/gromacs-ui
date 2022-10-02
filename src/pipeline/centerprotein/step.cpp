#include "step.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../model/project.h"

namespace Pipeline {

template<>
QString FactoryRegistration<CenterProtein::Step>::type =
  FactoryRegistration<CenterProtein::Step>::registerMethod("CenterProtein");
template<>
const QString FactoryRegistration<CenterProtein::Step>::name("Center Protein");

namespace CenterProtein {

using FileObject = ::Command::FileObject;
using InputOutput = ::Command::InputOutput;

Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : FactoryRegistration(
    project,
    {
      { InputOutput::Category::Configuration, {} },
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
