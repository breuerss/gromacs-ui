#include "removeconnectioncommand.h"
#include <QDebug>

namespace UndoRedo {

RemoveConnectionCommand::RemoveConnectionCommand(
  std::shared_ptr<Command::FileObject> newFileObject,
  Command::FileObjectConsumer* newConsumer,
  QUndoCommand* parent)
  : QUndoCommand(parent)
    , fileObject(newFileObject)
    , consumer(newConsumer)
{
}

void RemoveConnectionCommand::undo()
{
  consumer->connectTo(fileObject, false);
}

void RemoveConnectionCommand::redo()
{
  consumer->disconnectFrom(fileObject, false);
}

}
