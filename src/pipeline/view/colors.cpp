#include "colors.h"
#include <QMap>

namespace Pipeline { namespace View { namespace Colors {

QColor getColorFor(Step::Category category)
{
  using Category = Step::Category;
  const static QMap<Category, QColor> categoryColorMap = {
    { Category::DataProvider, Colors::Pink },
    { Category::PreProcess, Colors::Orange },
    { Category::Simulation, Colors::Blue },
    { Category::Analysis, Colors::Violet },
    { Category::PostProcess, Colors::Yellow },
  };

  return categoryColorMap[category];
}

QColor getColorFor(Command::InputOutput::Category category)
{
  using Category = Command::InputOutput::Category;
  const static QMap<Category, QColor> colors = {
    { Category::Coordinates, 0xff5b81e4 },
    { Category::Trajectory, 0xffe7911d },
    { Category::Energy, Colors::Green },
    { Category::Graph, Colors::Yellow },
    { Category::Forces, 0xff9974aa },
    { Category::Topology, 0xff00740a },
    { Category::Text, Colors::Grey },
    { Category::Configuration, Colors::Grey },

    //0xffc1584f
  };

  return colors[category];
}

QColor getColorFor(Command::FileObject::Type type)
{
  auto category = Command::FileObject::getCategoryFor(type);

  return getColorFor(category);
}

} } }

