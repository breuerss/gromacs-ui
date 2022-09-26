#include "changeserializablecommand.h"
#include <QDebug>
#include <QObject>

namespace UndoRedo {

ChangeSerializableCommand::ChangeSerializableCommand(
  Model::Serializable* newModel,
  const QString& newName,
  const QVariant& newValue,
  const QVariant& newOldValue,
  QUndoCommand* parent)
  : QUndoCommand(QObject::tr("Change property"), parent)
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

bool ChangeSerializableCommand::mergeWith(const QUndoCommand* newCommand)
{
  bool canMerge = false;
  if (newCommand->id() == id())
  {
    auto typedCommand = static_cast<const ChangeSerializableCommand*>(newCommand);
    if (typedCommand->name == name)
    {
      value = typedCommand->value;
      canMerge = true;
    }
  }
  return canMerge;
}

}
