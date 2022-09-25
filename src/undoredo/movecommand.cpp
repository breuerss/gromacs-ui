#include "movecommand.h"

#include <QGraphicsScene>
#include <QDebug>
#include "../pipeline/step.h"

namespace UndoRedo {

MoveCommand::MoveCommand(Node* newNode, const QPointF& newOldPos,
                         QUndoCommand* parent)
  : QUndoCommand(parent)
    , step(newNode->getStep())
    , oldPos(newOldPos)
    , newPos(newNode->pos())
{
}

void MoveCommand::undo()
{
  step->setLocation(oldPos);
}

void MoveCommand::redo()
{
  step->setLocation(newPos);
}

bool MoveCommand::mergeWith(const QUndoCommand *command)
{
  const MoveCommand *moveCommand = static_cast<const MoveCommand *>(command);
  auto item = moveCommand->step;

  if (step != item)
  {
    return false;
  }

  newPos = item->getLocation();

  return true;
}
}
