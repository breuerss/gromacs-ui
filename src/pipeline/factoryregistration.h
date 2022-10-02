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
  const static QString name;

protected:
  static QString type;

private:
  static QString registerMethod(const QString& newType)
  {
    Pipeline::StepFactory::registerMethod(newType, create);
    return newType;
  }

  static Pipeline::Step::Pointer
  create(std::shared_ptr<Model::Project> project)
  {
    return std::make_unique<StepType>(project);
  }

  QString getType() const override
  {
    return type;
  }

  QString getName() const override
  {
    return name;
  }
};


}

#endif
