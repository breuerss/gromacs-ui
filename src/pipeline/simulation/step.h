#ifndef PIPELINE_SIMULATION_STEP_H
#define PIPELINE_SIMULATION_STEP_H

#include "../step.h"

namespace Model {
class Project;
}

namespace Pipeline { namespace Simulation {

class Step : public Pipeline::Step {
public:
  Step(std::shared_ptr<Model::Project> project);
  QString getName() const override;
};

} }

#endif
