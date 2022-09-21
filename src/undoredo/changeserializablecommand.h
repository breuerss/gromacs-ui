#ifndef UNDO_REDO_CHANGE_SERIALIZABLE_COMMAND_H
#define UNDO_REDO_CHANGE_SERIALIZABLE_COMMAND_H

#include <QUndoCommand>
#include <QVariant>

#include "../model/serializable.h"

namespace UndoRedo {

class ChangeSerializableCommand : public QUndoCommand
{
public:
    ChangeSerializableCommand(
      Model::Serializable* newModel,
      const QString& newName,
      const QVariant& newValue,
      const QVariant& newOldValue,
      QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Model::Serializable* model;
    const QString name;
    const QVariant value;
    const QVariant oldValue;
};

}

#endif
