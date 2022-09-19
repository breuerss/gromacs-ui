#ifndef UNDO_STACK_H
#define UNDO_STACK_H

#include <QUndoStack>

class UndoStack : public QUndoStack
{
public:
  static UndoStack* getInstance();

private:
  UndoStack() = default;
};

#endif
