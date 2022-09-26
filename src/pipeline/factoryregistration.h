#ifndef PIPELINE_FACTORYREGISTRATION_H
#define PIPELINE_FACTORYREGISTRATION_H

#include "stepfactory.h"
#include "step.h"
#include <QString>
#include <memory>

namespace Pipeline {
template<typename StepType>
class FactoryRegistration {

protected:
  static QString type;

private:
  static bool registerMethod(const QString& newType)
  {
    type = newType;
    return Pipeline::StepFactory::registerMethod(newType, create);
  }
  static bool registered;
  static Pipeline::Step::Pointer
  create(std::shared_ptr<Model::Project> project)
  {
    return std::make_unique<StepType>(project);
  }
};

template<typename StepType>
QString FactoryRegistration<StepType>::type;


}

#endif
