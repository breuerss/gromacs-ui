#ifndef CONFIG_SUPPORTED_CONFIGS_H
#define CONFIG_SUPPORTED_CONFIGS_H

#include "simulation.h"
#include "pdb.h"

#include <memory>
#include <variant>

namespace Config {

using Type = std::variant<
  std::shared_ptr<Simulation>
  , std::shared_ptr<Pdb>
>;
}


#endif
