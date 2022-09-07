#ifndef PIPELINE_SOLVATE_STEP_H
#define PIPELINE_SOLVATE_STEP_H

#include "../step.h"

namespace Model {
class Project;
}

namespace Pipeline { namespace Solvate  {

class Step : public Pipeline::Step {

public:
  Step(std::shared_ptr<Model::Project> project = nullptr);
  QString getName() const override;
  QString getType() const override;
  Step::Pointer create(std::shared_ptr<Model::Project>) const override;
};

} }

#endif
