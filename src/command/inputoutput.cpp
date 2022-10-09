#include "inputoutput.h"

#include <QMap>

namespace Command {

QString InputOutput::toString(InputOutput::Category category)
{
  using Category = InputOutput::Category;
  const static QMap<Category, QString> map = {
    { Category::Unknown, "Unknown" },
    { Category::Coordinates, "Coordinates" },
    { Category::Trajectory, "Trajectory" },
    { Category::Energy, "Energy" },
    { Category::Velocities, "Velocities" },
    { Category::Forces, "Forces" },
    { Category::Topology, "Topology" },
    { Category::Text, "Text" },
    { Category::Graph, "Graph" },
    { Category::Configuration, "Configuration" },
  };

  return map[category];
}


}

