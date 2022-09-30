#ifndef PIPELINE_VIEW_TOOLTIPMANAGER_H
#define PIPELINE_VIEW_TOOLTIPMANAGER_H

#include "tooltip.h"

namespace Pipeline {
namespace View {

class TooltipManager {
public:
  static void show(Tooltip* tooltip, bool newExclusive = true);
  static void hide(Tooltip* tooltip);

private:
  static Tooltip* tooltip;
  static bool exclusive;

};
}
}

#endif
