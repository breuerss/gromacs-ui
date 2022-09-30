#include "step.h"
#include "configuration.h"

namespace Pipeline {

template<>
QString FactoryRegistration<TimeStepControl::Step>::type =
  FactoryRegistration<TimeStepControl::Step>::registerMethod("TimeStepControl");

namespace TimeStepControl {

Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : FactoryRegistration(
    project,
    { },
    {
    },
    std::make_shared<Configuration>(),
    nullptr,
    nullptr,
    Category::PostProcess
    )
{
}

QString Step::getName() const
{
  return "Time Step Control";
}

} }
