#include "step.h"
#include "configuration.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"

namespace Pipeline {

template<>
QString FactoryRegistration<CreateGromacsModel::Step>::type =
  FactoryRegistration<CreateGromacsModel::Step>::registerMethod("CreateGromacsModel");

namespace CreateGromacsModel {

using FileObject = ::Command::FileObject;
using InputOutput = ::Command::InputOutput;
Step::Step(std::shared_ptr<Model::Project> project)
  : FactoryRegistration(
    project,
    {
      { InputOutput::Category::Coordinates,
        {
          FileObject::Type::PDB, FileObject::Type::GRO
        }
      }
    },
    {
      FileObject::Type::GRO,
      FileObject::Type::TOP,
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
  return "Create GROMACS Model";
}

} }
