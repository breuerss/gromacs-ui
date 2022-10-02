#include "step.h"
#include "configuration.h"
#include "command.h"
#include "filenamegenerator.h"
#include <memory>
#include "../../command/fileobject.h"
#include "../stepfactory.h"

namespace Pipeline {

template<>
QString FactoryRegistration<Solvate::Step>::type =
  FactoryRegistration<Solvate::Step>::registerMethod("Solvate");
template<>
const QString FactoryRegistration<Solvate::Step>::name("Solvate");

namespace Solvate {

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
