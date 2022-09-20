#include "movecommand.h"

#include <QGraphicsScene>
#include <QDebug>

namespace UndoRedo {

MoveCommand::MoveCommand(QGraphicsItem* newNode, const QPointF& newOldPos,
                         QUndoCommand* parent)
  : QUndoCommand(parent)
    , node(newNode)
    , oldPos(newOldPos)
    , newPos(newNode->pos())
{
}

void MoveCommand::undo()
{
  node->setPos(oldPos);
  node->scene()->update();
}

void MoveCommand::redo()
{
  node->setPos(newPos);
}

bool MoveCommand::mergeWith(const QUndoCommand *command)
{
  const MoveCommand *moveCommand = static_cast<const MoveCommand *>(command);
  auto item = moveCommand->node;

  if (node != item)
  {
    return false;
  }

  newPos = item->pos();

  return true;
}
}
