#ifndef UNDO_REDO_HELPER_H
#define UNDO_REDO_HELPER_H

#include "stack.h"
#include "addconnectioncommand.h"
#include "removeconnectioncommand.h"
#include "../command/fileobjectconsumer.h"

namespace UndoRedo { namespace Helper {

void connectTo(
  Command::FileObjectConsumer* consumer,
  Command::FileObject::Pointer inputObject
  );

void disconnectFrom(
  Command::FileObjectConsumer* consumer,
  Command::FileObject::Pointer inputObject
  );

} }

#endif
