#include "uiupdater.h"

UiUpdater* UiUpdater::getInstance()
{
  static UiUpdater instance;

  return &instance;
}
