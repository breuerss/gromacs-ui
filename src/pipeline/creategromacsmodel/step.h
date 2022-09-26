#ifndef PIPELINE_CREATEGROMACSMODEL_STEP_H
#define PIPELINE_CREATEGROMACSMODEL_STEP_H

#include "../factoryregistration.h"

namespace Pipeline { namespace CreateGromacsModel {

class Step : public FactoryRegistration<Step> {

public:
  Step(std::shared_ptr<Model::Project> project = nullptr);
  QString getName() const override;
};

} }

#endif
