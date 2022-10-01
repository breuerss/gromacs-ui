#ifndef UNDO_REDO_REMOVE_CONNECTION_COMMAND_H
#define UNDO_REDO_REMOVE_CONNECTION_COMMAND_H

#include "../command/types.h"
#include <QUndoCommand>
#include <memory>

namespace Command {
class FileObject;
class FileObjectConsumer;
}

namespace UndoRedo {

class RemoveConnectionCommand : public QUndoCommand
{
public:
    RemoveConnectionCommand(
      const Command::Data& newData,
      Command::FileObjectConsumer* newConsumer,
      QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Command::Data data;
    Command::FileObjectConsumer* consumer;
};

}

#endif
