#include "configuration.h"
#include "../ui/pdbcode.h"
#include <memory>

namespace Pipeline { namespace PdbDownload {

QWidget* Configuration::getUI()
{
  return new PdbCode(this);
}

} }
