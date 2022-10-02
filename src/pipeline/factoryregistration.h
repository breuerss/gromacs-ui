#ifndef PIPELINE_FACTORYREGISTRATION_H
#define PIPELINE_FACTORYREGISTRATION_H

#include "stepfactory.h"
#include "step.h"
#include <QString>
#include <QDebug>
#include <memory>

namespace Pipeline {

template<typename StepType>
class FactoryRegistration : public Step {

public:
  using Step::Step;

  struct Config {
    QString type;
    QString name;
    Category category;
  };

  const static Config config;

private:
  static Config registerMethod(const Config& newConfig)
  {
    Pipeline::StepFactory::registerMethod(newConfig.type, create);
    return newConfig;
  }

  static Pipeline::Step::Pointer
  create(std::shared_ptr<Model::Project> project)
  {
    return std::make_unique<StepType>(project);
  }

  QString getType() const override
  {
    return config.type;
  }

  QString getName() const override
  {
    return config.name;
  }

  Category getCategory() const override
  {
    return config.category;
  }
};


}

#endif
