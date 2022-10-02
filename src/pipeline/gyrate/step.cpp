#include "step.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"

namespace Pipeline {

using Registration = Pipeline::FactoryRegistration<Gyrate::Step>;
template<>
QString Registration::type =
  Registration::registerMethod("Gyrate");
template<>
const QString Registration::name("Radius of Gyration");
template<>
const Step::Category Registration::category = Step::Category::Analysis;

namespace Gyrate {

using FileObject = ::Command::FileObject;
using InputOutput = ::Command::InputOutput;
Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : FactoryRegistration<Step>(
    project,
    {
      { InputOutput::Category::Trajectory, { FileObject::Type::XTC } },
      { InputOutput::Category::Topology, { FileObject::Type::TPR } },
    },
    {
      FileObject::Type::XVG,
    },
    nullptr,
    std::make_shared<Command>(),
    std::make_shared<FileNameGenerator>(project)
    )
{
}

} }
