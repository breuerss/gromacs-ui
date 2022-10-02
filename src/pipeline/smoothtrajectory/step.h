#ifndef PIPELINE_SMOOTHTRAJECTORY_STEP_H
#define PIPELINE_SMOOTHTRAJECTORY_STEP_H

#include "../factoryregistration.h"

namespace Pipeline { namespace SmoothTrajectory {

class Step : public FactoryRegistration<Step> {

public:
  Step(std::shared_ptr<Model::Project> project = nullptr);
};

} }

#endif
