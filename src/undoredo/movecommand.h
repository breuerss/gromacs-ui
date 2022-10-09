#ifndef UNDO_REDO_MOVE_COMMAND_H
#define UNDO_REDO_MOVE_COMMAND_H

#include <QUndoCommand>
#include <QPointF>
#include <memory>

namespace Pipeline {
class Step;
namespace View {
class Node;
}
}

namespace UndoRedo {

class MoveCommand : public QUndoCommand
{
public:
  enum { Id = 1234 };

  MoveCommand(Pipeline::View::Node* node, const QPointF& oldPos,
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
