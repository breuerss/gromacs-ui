#include "configuration.h"
#include "../ui/pdbcode.h"
#include <memory>

namespace Pipeline { namespace PdbDownload {

Configuration::Configuration()
{
  connectAllSignals();
}

QWidget* Configuration::getUI()
{
  return new PdbCode(this);
}

} }
