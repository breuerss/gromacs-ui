#include "addconnectioncommand.h"
#include <QDebug>

namespace UndoRedo {

AddConnectionCommand::AddConnectionCommand(
  std::shared_ptr<Command::FileObject> newFileObject,
  Command::FileObjectConsumer* newConsumer,
  QUndoCommand* parent)
  : QUndoCommand(parent)
    , fileObject(newFileObject)
    , consumer(newConsumer)
{
}

void AddConnectionCommand::undo()
{
  consumer->disconnectFrom(fileObject, false);
}

void AddConnectionCommand::redo()
{
  consumer->connectTo(fileObject, false);
}

}
