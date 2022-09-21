#include "changeserializablecommand.h"
#include <QDebug>

namespace UndoRedo {

ChangeSerializableCommand::ChangeSerializableCommand(
  Model::Serializable* newModel,
  const QString& newName,
  const QVariant& newValue,
  const QVariant& newOldValue,
  QUndoCommand* parent)
  : QUndoCommand(parent)
    , model(newModel)
    , name(newName)
    , value(newValue)
    , oldValue(newOldValue)
{
}

void ChangeSerializableCommand::undo()
{
  model->setProperty(name.toStdString().c_str(), oldValue, false);
}

void ChangeSerializableCommand::redo()
{
  model->setProperty(name.toStdString().c_str(), value, false);
}

}
