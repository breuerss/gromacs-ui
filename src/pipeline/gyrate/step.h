#ifndef PIPELINE_GYRATE_STEP_H
#define PIPELINE_GYRATE_STEP_H

#include "../factoryregistration.h"

namespace Pipeline { namespace Gyrate {

class Step : public FactoryRegistration<Step> {

public:
  Step(std::shared_ptr<Model::Project> project = nullptr);
};

} }

#endif
