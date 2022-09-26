#include "step.h"
#include "configuration.h"
#include "command.h"
#include "filenamegenerator.h"
#include <memory>
#include "../../command/fileobject.h"
#include "../stepfactory.h"

namespace Pipeline {

template<>
bool FactoryRegistration<Solvate::Step>::registered =
  FactoryRegistration<Solvate::Step>::registerMethod("Solvate");

namespace Solvate {

using FileObject = ::Command::FileObject;
Step::Step(std::shared_ptr<Model::Project> project)
  : FactoryRegistration(
    project,
    {
      { FileObject::Category::Coordinates,
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

QString Step::getName() const
{
  return "Solvate";
}

} }
