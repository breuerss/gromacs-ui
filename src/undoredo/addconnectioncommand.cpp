#include "addconnectioncommand.h"
#include "../command/fileobjectconsumer.h"
#include "../command/fileobject.h"
#include <QDebug>
#include <QObject>

namespace UndoRedo {

AddConnectionCommand::AddConnectionCommand(
  const Command::Data& newData,
  Command::FileObjectConsumer* newConsumer,
  QUndoCommand* parent)
  : QUndoCommand(QObject::tr("Add connection"), parent)
    , data(newData)
    , consumer(newConsumer)
{
}

void AddConnectionCommand::undo()
{
  consumer->disconnectFrom(data);
}

void AddConnectionCommand::redo()
{
  consumer->connectTo(data);
}

}
