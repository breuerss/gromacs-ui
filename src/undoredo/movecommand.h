#ifndef UNDO_REDO_MOVE_COMMAND_H
#define UNDO_REDO_MOVE_COMMAND_H

#include <QUndoCommand>
#include "../pipeline/view/node.h"

namespace UndoRedo {

class MoveCommand : public QUndoCommand
{
  using Node = Pipeline::View::Node;
public:
    enum { Id = 1234 };

    MoveCommand(QGraphicsItem* node, const QPointF& oldPos,
                QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand* command) override;
    int id() const override { return Id; }

private:
    QGraphicsItem* node;
    QPointF oldPos;
    QPointF newPos;
};

}

#endif
