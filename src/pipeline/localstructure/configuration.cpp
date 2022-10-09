#include "configuration.h"
#include "../ui/fileinput.h"
#include <memory>

namespace Pipeline { namespace LocalStructure {

Configuration::Configuration()
{
  connectAllSignals();
}

QWidget* Configuration::getUI()
{
  return new FileInput(this);
}

QString Configuration::toString()
{
  return tr("Source File Name") + ": " + fileName;
}

} }
