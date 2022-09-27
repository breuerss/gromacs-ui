#include "step.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"

namespace Pipeline {

template<>
QString FactoryRegistration<Gyrate::Step>::type =
  FactoryRegistration<Gyrate::Step>::registerMethod("Gyrate");

namespace Gyrate {

using FileObject = ::Command::FileObject;
Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : FactoryRegistration<Step>(
    project,
    {
      { FileObject::Category::Trajectory, { FileObject::Type::XTC } },
      { FileObject::Category::Topology, { FileObject::Type::TPR } },
    },
    {
      FileObject::Type::XVG,
    },
    nullptr,
    std::make_shared<Command>(),
    std::make_shared<FileNameGenerator>(project),
    Category::Analysis
    )
{
}

QString Step::getName() const
{
  return "Gyrate";
}

} }
