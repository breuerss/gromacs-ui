#include "step.h"
#include "configuration.h"
#include "command.h"
#include "filenamegenerator.h"

namespace Pipeline {

template<>
QString FactoryRegistration<Neutralise::Step>::type =
  FactoryRegistration<Neutralise::Step>::registerMethod("Neutralise");
template<>
const QString FactoryRegistration<Neutralise::Step>::name("Neutralise");

namespace Neutralise {

using FileObject = ::Command::FileObject;
using InputOutput = ::Command::InputOutput;
Step::Step(std::shared_ptr<Model::Project> project)
  : FactoryRegistration(
    project,
    {
      { InputOutput::Category::Coordinates,
        {
          FileObject::Type::GRO
        }
      }
    },
    {
      FileObject::Type::GRO,
    },
    std::make_shared<Configuration>(),
    std::make_shared<Command>(),
    std::make_shared<FileNameGenerator>(project),
    Category::PreProcess
    )
{
}

} }
