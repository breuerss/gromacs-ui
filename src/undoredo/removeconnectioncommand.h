#ifndef UNDO_REDO_REMOVE_CONNECTION_COMMAND_H
#define UNDO_REDO_REMOVE_CONNECTION_COMMAND_H

#include <QUndoCommand>
#include "../command/fileobjectconsumer.h"
#include "../command/fileobject.h"

namespace UndoRedo {

class RemoveConnectionCommand : public QUndoCommand
{
public:
    RemoveConnectionCommand(
      std::shared_ptr<Command::FileObject> newFileObject,
      Command::FileObjectConsumer* newConsumer,
      QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    std::shared_ptr<Command::FileObject> fileObject;
    Command::FileObjectConsumer* consumer;
};

}

#endif
