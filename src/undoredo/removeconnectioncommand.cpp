#include "removeconnectioncommand.h"
#include "../command/fileobjectconsumer.h"
#include <QDebug>
#include <QObject>

namespace UndoRedo {

RemoveConnectionCommand::RemoveConnectionCommand(
  const Command::Data& newData,
  Command::FileObjectConsumer* newConsumer,
  QUndoCommand* parent)
  : QUndoCommand(QObject::tr("Remove connection"), parent)
    , data(newData)
    , consumer(newConsumer)
{
}

void RemoveConnectionCommand::undo()
{
  consumer->connectTo(data);
}

void RemoveConnectionCommand::redo()
{
  consumer->disconnectFrom(data);
}

}
