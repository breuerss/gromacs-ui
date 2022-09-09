#include "colors.h"
#include <QMap>

namespace Pipeline { namespace View { namespace Colors {

QColor getColorFor(Step::Category category)
{
  using Category = Step::Category;
  const static QMap<Category, QColor> categoryColorMap = {
    { Category::DataProvider, Colors::Pink },
    { Category::Preprocess, Colors::Orange },
    { Category::Simulation, Colors::Blue },
    { Category::Viewer, Colors::Violet },
  };

  return categoryColorMap[category];
}

QColor getColorFor(Command::FileObject::Category category)
{
  using Category = Command::FileObject::Category;
  const static QMap<Category, QColor> colors = {
    { Category::Coordinates, 0xff5b81e4 },
    { Category::Trajectory, 0xffe7911d },
    { Category::Energy, 0xffc0c753 },
    { Category::Forces, 0xff9974aa },

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

