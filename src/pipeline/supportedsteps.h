#ifndef PIPELINE_SUPPORTED_STEPS_H
#define PIPELINE_SUPPORTED_STEPS_H

#include "../command/runsimulation.h"

#include <memory>
#include <variant>

namespace Pipeline {

using StepType = std::variant<
  std::shared_ptr<Command::RunSimulation>
>;
}


#endif
