#ifndef COMMAND_INPUT_OUTPUT_H
#define COMMAND_INPUT_OUTPUT_H

#include <QString>

namespace Command {

class InputOutput {
public:
  enum class Category {
    Unknown,
    Coordinates,
    Trajectory,
    Energy,
    Velocities,
    Forces,
    Topology,
    Text,
    Graph,
    Configuration,
  };

  static QString toString(InputOutput::Category);
};

}

#endif
