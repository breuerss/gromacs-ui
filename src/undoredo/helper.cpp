#include "helper.h"

namespace UndoRedo { namespace Helper {

void connectTo(
  Command::FileObjectConsumer* consumer,
  const Command::Data& inputObject
  )
{
  UndoRedo::Stack::getInstance()->push(
    new UndoRedo::AddConnectionCommand(inputObject, consumer)
  );
}

void disconnectFrom(
  Command::FileObjectConsumer* consumer,
  const Command::Data& inputObject
  )
{
  UndoRedo::Stack::getInstance()->push(
    new UndoRedo::RemoveConnectionCommand(inputObject, consumer)
  );
}

} }
