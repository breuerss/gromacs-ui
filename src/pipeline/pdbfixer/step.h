#ifndef PIPELINE_PDBFIXER_STEP_H
#define PIPELINE_PDBFIXER_STEP_H

#include "../factoryregistration.h"

namespace Pipeline { namespace PdbFixer {

class Step : public FactoryRegistration<Step> {

public:
  Step(std::shared_ptr<Model::Project> project = nullptr);
};

} }

#endif
