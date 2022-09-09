#ifndef PIPELINE_STEPFACTORY_H
#define PIPELINE_STEPFACTORY_H

#include <memory>
#include <QString>
#include <map>

namespace Model {
class Project;
}

namespace Pipeline {

class Step;

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
