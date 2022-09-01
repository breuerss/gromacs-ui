#include "simulation.h"
#include "../command/fileobject.h"
#include "../command/runsimulation.h"
#include "../config/simulation.h"

namespace Pipeline {

Simulation::Simulation(
    std::shared_ptr<Model::Project> project
  )
  : Step(
    {
      { Command::FileObject::Category::Coordinates, { Command::FileObject::Type::GRO } }
    },
    {
      Command::FileObject::Type::GRO,
      Command::FileObject::Type::XTC
    },
    std::make_shared<Config::Simulation>(),
    std::make_shared<Command::RunSimulation>(project),
    nullptr, // TODO filename generator for simulation
    Category::Simulation
    )
{
}

QString Simulation::getName() const
{
  return "mdrun simulation";
}


}
