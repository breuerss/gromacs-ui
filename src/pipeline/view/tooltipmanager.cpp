#include "tooltipmanager.h"

namespace Pipeline {
namespace View {

Tooltip* TooltipManager::tooltip = nullptr;

void TooltipManager::show(Tooltip* newTooltip)
{
  if (tooltip)
  {
    tooltip->hide();
  }
  tooltip = newTooltip;
  tooltip->show();
}

}
}
