#include "configuration.h"
#include "../ui/neutralisesettings.h"
#include <QMetaProperty>

namespace Pipeline { namespace Neutralise {

QWidget* Configuration::getUI()
{
  return new NeutraliseSettings(this);
}

} }
