#include "addconnectioncommand.h"
#include <QDebug>
#include <QObject>

namespace UndoRedo {

AddConnectionCommand::AddConnectionCommand(
  std::shared_ptr<Command::FileObject> newFileObject,
  Command::FileObjectConsumer* newConsumer,
  QUndoCommand* parent)
  : QUndoCommand(QObject::tr("Add connection"), parent)
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
