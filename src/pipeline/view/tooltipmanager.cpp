#include "tooltipmanager.h"

namespace Pipeline {
namespace View {

Tooltip* TooltipManager::tooltip = nullptr;
bool TooltipManager::exclusive = false;

void TooltipManager::show(Tooltip* newTooltip, bool newExclusive)
{
  if (exclusive)
  {
    return;
  }

  if (tooltip)
  {
    tooltip->hide();
  }
  exclusive = newExclusive;
  tooltip = newTooltip;
  tooltip->show();
}

void TooltipManager::hide(Tooltip* newTooltip)
{
  if (tooltip == newTooltip)
  {
    tooltip->hide();
    tooltip = nullptr;
    exclusive = false;
  }
}

}
}
