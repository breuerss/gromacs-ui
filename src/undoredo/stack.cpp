#include "stack.h"

namespace UndoRedo {

Stack* Stack::getInstance()
{
  static Stack instance;

  return &instance;
}

}
