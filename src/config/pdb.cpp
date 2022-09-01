#include "pdb.h"
#include "../ui/pdbcode.h"
#include <memory>

namespace Config {

QWidget* Pdb::getUI()
{
  return new PdbCode(shared_from_this());
}

}
