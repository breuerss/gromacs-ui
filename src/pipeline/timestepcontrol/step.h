#ifndef PIPELINE_TIMESTEPCONTROL_STEP_H
#define PIPELINE_TIMESTEPCONTROL_STEP_H

#include "../factoryregistration.h"

namespace Pipeline { namespace TimeStepControl {

class Step : public FactoryRegistration<Step> {

public:
  Step(std::shared_ptr<Model::Project> project = nullptr);
  QString getName() const override;
};

} }

#endif
