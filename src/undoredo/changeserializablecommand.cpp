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
  qDebug() << "undo" << oldValue;
  model->setProperty(name.toStdString().c_str(), oldValue, false);
}

void ChangeSerializableCommand::redo()
{
  qDebug() << "redo" << value;
  model->setProperty(name.toStdString().c_str(), value, false);
}

}
