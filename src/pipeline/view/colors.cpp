#include "colors.h"

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

} } }

