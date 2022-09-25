#include "stepfactory.h"

#include "../model/project.h"
#include <memory>

namespace Pipeline {

QMap<QString, StepFactory::CreateMethod> StepFactory::factoryMap;

bool StepFactory::registerMethod(
  const QString& type, 
  StepFactory::CreateMethod funcCreate)
{
  if (!factoryMap.contains(type))
  {
    factoryMap[type] = funcCreate;
    return true;
  }
  return false;
}

std::shared_ptr<Step> StepFactory::create(
  const QString& type,
  std::shared_ptr<Model::Project> project
  )
{
  if (factoryMap.contains(type))
  {
    return factoryMap[type](project);
  }

  return nullptr;
}

}
