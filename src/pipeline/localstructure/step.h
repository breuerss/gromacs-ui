#ifndef PIPELINE_LOCAL_STRUCTURE_STEP_H
#define PIPELINE_LOCAL_STRUCTURE_STEP_H

#include "../factoryregistration.h"

namespace Pipeline { namespace LocalStructure {

class Step : public FactoryRegistration<Step> {

public:
  Step(std::shared_ptr<Model::Project> project = nullptr);
};

} }

#endif
