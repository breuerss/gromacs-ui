#include "helper.h"

namespace UndoRedo { namespace Helper {

void connectTo(
  Command::FileObjectConsumer* consumer,
  Command::FileObject::Pointer inputObject
  )
{
  UndoRedo::Stack::getInstance()->push(
    new UndoRedo::AddConnectionCommand(inputObject, consumer)
  );
}

void disconnectFrom(
  Command::FileObjectConsumer* consumer,
  Command::FileObject::Pointer inputObject
  )
{
  UndoRedo::Stack::getInstance()->push(
    new UndoRedo::RemoveConnectionCommand(inputObject, consumer)
  );
}

} }
