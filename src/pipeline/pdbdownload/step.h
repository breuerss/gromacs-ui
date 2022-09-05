#ifndef PIPELINE_PDBDOWNLOAD_STEP_H
#define PIPELINE_PDBDOWNLOAD_STEP_H

#include "../step.h"

namespace Model {
class Project;
}

namespace Pipeline { namespace PdbDownload {

class Step : public Pipeline::Step {

public:
  Step(std::shared_ptr<Model::Project> project);
  QString getName() const override;
};

} }

#endif
