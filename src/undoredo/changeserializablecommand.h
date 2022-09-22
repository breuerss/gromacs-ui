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

    enum { Id = 2 };

    void undo() override;
    void redo() override;
    int id() const override { return Id; }
    bool mergeWith(const QUndoCommand*) override;

private:
    Model::Serializable* model;
    const QString name;
    QVariant value;
    const QVariant oldValue;
};

}

#endif
