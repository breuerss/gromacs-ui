#ifndef PIPELINE_CREATEBOX_STEP_H
#define PIPELINE_CREATEBOX_STEP_H

#include "../factoryregistration.h"

namespace Pipeline { namespace CreateBox  {

class Step : public FactoryRegistration<Step> {

public:
  Step(std::shared_ptr<Model::Project> project = nullptr);
  QString getName() const override;
};

} }

#endif
