#ifndef PIPELINE_CREATEBOX_STEP_H
#define PIPELINE_CREATEBOX_STEP_H

#include "../step.h"

namespace Model {
class Project;
}

namespace Pipeline { namespace CreateBox  {

class Step : public Pipeline::Step {

public:
  Step(std::shared_ptr<Model::Project> project = nullptr);
  QString getName() const override;
  QString getType() const override;
  Step::Pointer create(std::shared_ptr<Model::Project>) const override;
};

} }

#endif
