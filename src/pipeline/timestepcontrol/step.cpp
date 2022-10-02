#include "step.h"
#include "configuration.h"
#include "../../command/fileobjectprovider.h"

namespace Pipeline {

using Registration = FactoryRegistration<TimeStepControl::Step>;
template<>
QString Registration::type = Registration::registerMethod("TimeStepControl");
template<>
const QString Registration::name("Time Step Control");
template<>
const Step::Category Registration::category = Step::Category::PostProcess;

namespace TimeStepControl {

Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : FactoryRegistration(
    project,
    { },
    {
      std::make_shared<Configuration>(),
    },
    std::make_shared<Configuration>(),
    nullptr,
    nullptr
    )
{
  auto& provides = getFileObjectProvider()->provides();
  provides.clear();
  provides.append(configuration);
}

} }
