#include "removeconnectioncommand.h"
#include "../command/fileobjectconsumer.h"
#include <QDebug>
#include <QObject>

namespace UndoRedo {

RemoveConnectionCommand::RemoveConnectionCommand(
  std::shared_ptr<Command::FileObject> newFileObject,
  Command::FileObjectConsumer* newConsumer,
  QUndoCommand* parent)
  : QUndoCommand(QObject::tr("Remove connection"), parent)
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
