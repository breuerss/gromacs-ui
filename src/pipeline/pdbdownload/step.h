#ifndef PIPELINE_PDBDOWNLOAD_STEP_H
#define PIPELINE_PDBDOWNLOAD_STEP_H

#include "../step.h"
#include "../../model/project.h"

namespace Pipeline { namespace PdbDownload {

class Step : public Pipeline::Step {

public:
  Step(std::shared_ptr<Model::Project> project);
  QString getName() const override;
};

} }

#endif
