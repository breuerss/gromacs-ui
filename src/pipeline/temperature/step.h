#ifndef PIPELINE_TEMPERATURE_STEP_H
#define PIPELINE_TEMPERATURE_STEP_H

#include "../factoryregistration.h"

namespace Pipeline { namespace Temperature {

class Step : public FactoryRegistration<Step> {

public:
  Step(std::shared_ptr<Model::Project> project = nullptr);
};

} }

#endif
