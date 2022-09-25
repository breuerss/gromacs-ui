#include "step.h"
#include "command.h"
#include "filenamegenerator.h"
#include "../../command/fileobject.h"
#include "../stepfactory.h"

namespace Pipeline { namespace Gyrate {

QString Step::type = "Gyrate";
bool Step::registered = StepFactory::registerMethod(Step::type, Step::create);

using FileObject = ::Command::FileObject;
Step::Step(
    std::shared_ptr<Model::Project> project
  )
  : Pipeline::Step(
    {
      { FileObject::Category::Trajectory, { FileObject::Type::XTC } },
      { FileObject::Category::Topology, { FileObject::Type::TPR } },
    },
    {
      FileObject::Type::XVG,
    },
    nullptr,
    std::make_shared<Command>(),
    std::make_shared<FileNameGenerator>(project),
    Category::Analysis
    )
{
}

QString Step::getName() const
{
  return "Gyrate";
}

QString Step::getType() const
{
  return type;
}

Step::Pointer
Step::create(std::shared_ptr<Model::Project> project)
{
  return std::make_unique<Step>(project);
}


} }
