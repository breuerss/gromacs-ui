#include "step.h"
#include "configuration.h"
#include "command.h"
#include "filenamegenerator.h"

namespace Pipeline {

template<>
bool FactoryRegistration<Neutralise::Step>::registered =
  FactoryRegistration<Neutralise::Step>::registerMethod("Neutralise");

namespace Neutralise {

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
  return "Neutralise";
}

} }
