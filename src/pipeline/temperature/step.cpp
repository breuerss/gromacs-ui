#include "step.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"

namespace Pipeline {

using Registration = Pipeline::FactoryRegistration<Temperature::Step>;
template<>
const Registration::Config Registration::config = Registration::registerMethod({
  "Temperature", "Temperature", Step::Category::Analysis
});

namespace Temperature {

using FileObject = ::Command::FileObject;
using InputOutput = ::Command::InputOutput;
Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : FactoryRegistration<Step>(
    project,
    {
      { InputOutput::Category::Energy, { FileObject::Type::EDR } },
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
