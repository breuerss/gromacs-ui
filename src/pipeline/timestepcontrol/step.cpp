#include "step.h"
#include "configuration.h"
#include "../../command/fileobjectprovider.h"

namespace Pipeline {

template<>
QString FactoryRegistration<TimeStepControl::Step>::type =
  FactoryRegistration<TimeStepControl::Step>::registerMethod("TimeStepControl");
template<>
const QString FactoryRegistration<TimeStepControl::Step>::name("Time Step Control");

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
    nullptr,
    Category::PostProcess
    )
{
  auto& provides = getFileObjectProvider()->provides();
  provides.clear();
  provides.append(configuration);
}

} }
