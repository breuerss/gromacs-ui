#ifndef PIPELINE_STEPFACTORY_H
#define PIPELINE_STEPFACTORY_H

#include "src/model/project.h"
#include "step.h"
#include <memory>
#include <QString>
#include <map>

namespace Pipeline {

class StepFactory
{
public:
  static StepFactory* getInstance();
  std::shared_ptr<Step> createFromString(
    const QString& typeName,
    std::shared_ptr<Model::Project>
    );

private:
  StepFactory();
  std::map<QString, std::unique_ptr<Step>> factoryMap;

};

}

#endif
