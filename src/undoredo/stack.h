#ifndef UNDO_REDO_STACK_H
#define UNDO_REDO_STACK_H

#include <QUndoStack>

namespace UndoRedo {

class Stack : public QUndoStack
{
public:
  static Stack* getInstance();

private:
  Stack() = default;
};

}
#endif
