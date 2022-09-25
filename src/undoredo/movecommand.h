#ifndef UNDO_REDO_MOVE_COMMAND_H
#define UNDO_REDO_MOVE_COMMAND_H

#include "../pipeline/view/node.h"
#include <QUndoCommand>

namespace UndoRedo {

class MoveCommand : public QUndoCommand
{
  using Node = Pipeline::View::Node;
public:
    enum { Id = 1234 };

    MoveCommand(Node* node, const QPointF& oldPos,
                QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand* command) override;
    int id() const override { return Id; }

private:
    std::shared_ptr<Pipeline::Step> step;
    QPointF oldPos;
    QPointF newPos;
};

}

#endif
