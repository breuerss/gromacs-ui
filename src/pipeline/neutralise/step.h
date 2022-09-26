#ifndef PIPELINE_NEUTRALISE_STEP_H
#define PIPELINE_NEUTRALISE_STEP_H

#include "../factoryregistration.h"

namespace Pipeline { namespace Neutralise  {

class Step : public FactoryRegistration<Step> {

public:
  Step(std::shared_ptr<Model::Project> project = nullptr);
  QString getName() const override;
};

} }

#endif
