#include "configuration.h"
#include "../uiupdater.h"

namespace Config {

void Configuration::showUI(bool show)
{
  QWidget* widget = nullptr;
  if (show)
  {
    widget = getUI();
  }
  UiUpdater::getInstance()->showConfigUI(widget);
}

}
