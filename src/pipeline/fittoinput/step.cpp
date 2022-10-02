#include "step.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../model/project.h"

namespace Pipeline {

using Registration = FactoryRegistration<FitToInput::Step>;
template<>
QString Registration::type = Registration::registerMethod("FitToInput");
template<>
const QString Registration::name("Fit to Input");
template<>
const Step::Category Registration::category = Step::Category::PostProcess;

namespace FitToInput {


using FileObject = ::Command::FileObject;
using InputOutput = ::Command::InputOutput;

Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : FactoryRegistration(
    project,
    {
      { InputOutput::Category::Configuration, {} },
      { InputOutput::Category::Coordinates, { FileObject::Type::GRO } },
      { InputOutput::Category::Trajectory, { FileObject::Type::XTC } },
    },
    {
      FileObject::Type::XTC
    },
    nullptr,
    std::make_shared<Command>(),
    std::make_shared<FileNameGenerator>(project)
    )
{
}

} }
