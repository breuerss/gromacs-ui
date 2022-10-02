#ifndef PIPELINE_SOLVATE_STEP_H
#define PIPELINE_SOLVATE_STEP_H

#include "../factoryregistration.h"

namespace Pipeline { namespace Solvate  {

class Step : public FactoryRegistration<Step> {

public:
  Step(std::shared_ptr<Model::Project> project = nullptr);
};

} }

#endif
