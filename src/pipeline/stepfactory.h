#ifndef PIPELINE_STEPFACTORY_H
#define PIPELINE_STEPFACTORY_H

#include <memory>
#include <QString>
#include <QMap>

namespace Model {
class Project;
}

namespace Pipeline {

class Step;

class StepFactory
{
public:
  using CreateMethod = std::shared_ptr<Step>(*)(
    std::shared_ptr<Model::Project>
    );

  static void registerMethod(const QString& name, CreateMethod funcCreate);
  static std::shared_ptr<Step> create(
    const QString& typeName,
    std::shared_ptr<Model::Project>
    );

private:
  StepFactory() = delete;
  static QMap<QString, CreateMethod>& getFactoryMap();
};

}

#endif
