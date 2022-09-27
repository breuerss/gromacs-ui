#include "stepfactory.h"

#include "../model/project.h"
#include <memory>

namespace Pipeline {

bool StepFactory::registerMethod(
  const QString& type, 
  StepFactory::CreateMethod funcCreate)
{
  if (!getFactoryMap().contains(type))
  {
    getFactoryMap()[type] = funcCreate;
    return true;
  }
  return false;
}

std::shared_ptr<Step> StepFactory::create(
  const QString& type,
  std::shared_ptr<Model::Project> project
  )
{
  if (getFactoryMap().contains(type))
  {
    return getFactoryMap()[type](project);
  }

  return nullptr;
}

QMap<QString, StepFactory::CreateMethod>& StepFactory::getFactoryMap()
{
  static QMap<QString, StepFactory::CreateMethod> map;

  return map;
}

}
