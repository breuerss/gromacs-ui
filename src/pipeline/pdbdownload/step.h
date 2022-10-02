#ifndef PIPELINE_PDBDOWNLOAD_STEP_H
#define PIPELINE_PDBDOWNLOAD_STEP_H

#include "../factoryregistration.h"

namespace Pipeline { namespace PdbDownload {

class Step : public FactoryRegistration<Step> {

public:
  Step(std::shared_ptr<Model::Project> project = nullptr);
};

} }

#endif
