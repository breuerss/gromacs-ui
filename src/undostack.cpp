#include "undostack.h"

UndoStack* UndoStack::getInstance()
{
  static UndoStack instance;

  return &instance;
}
