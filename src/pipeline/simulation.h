#ifndef PIPELINE_SIMULATION_H
#define PIPELINE_SIMULATION_H

#include "step.h"
#include "../model/project.h"

namespace Pipeline {

class Simulation : public Step {
public:
  Simulation(std::shared_ptr<Model::Project> project);
  QString getName() const override;
};

}

#endif
