#include "serializable.h"
#include "../undoredo/stack.h"
#include "../undoredo/changeserializablecommand.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QJsonObject>

namespace Model {

void Serializable::connectAllSignals()
{
  const QMetaObject* metaObj = metaObject();
  if (metaObj)
  {
    int numMethods = metaObj->methodCount();
    int firstMethod = metaObj->methodOffset();
    int anyChangedMethodIndex = metaObj->indexOfMethod("anyChanged()");
    auto anyChangedMethod = metaObj->method(anyChangedMethodIndex);
    for (int i = firstMethod; i < numMethods; i++)
    {
      QMetaMethod mm = metaObj->method(i);
      if (mm.methodType() == QMetaMethod::Signal &&
          anyChangedMethodIndex != i)
      {
        connect(this, mm, this, anyChangedMethod);
      }
    }
  }
}

bool Serializable::setProperty(const char* name, const QVariant& value, bool createUndoRedo)
{
  auto oldValue = property(name);
  if (createUndoRedo && oldValue != value)
  {
    qDebug() << "creating undo command" << value << oldValue;
    UndoRedo::Stack::getInstance()->push(new UndoRedo::ChangeSerializableCommand(
        this, name, value, oldValue));
  }
  else
  {
    return QObject::setProperty(name, value);
  }
  return true;
}

QString Serializable::getSignalStringForProperty(const QString& name)
{
  const QMetaMethod s = getSignalForProperty(name);

  QString signal = QString("2") + s.methodSignature();

  return signal;
}

QMetaMethod Serializable::getSignalForProperty(const QString& name)
{
  QMetaMethod signal;
  const auto senderMeta = metaObject();
  const int index = senderMeta->indexOfProperty(name.toStdString().c_str());
  if (index != -1) {
    const auto p = senderMeta->property(index);
    if ( p.hasNotifySignal() ) {
      signal = p.notifySignal();
    }
  }

  return signal;
}

QJsonObject &operator<<(QJsonObject &out, const std::shared_ptr<Serializable> model)
{
  const QMetaObject *metaobject = model->metaObject();
  int count = metaobject->propertyCount();
  for (int i = 0; i < count; ++i) {
    QMetaProperty metaproperty = metaobject->property(i);
    const char *name = metaproperty.name();
    QVariant value = model->property(name);

    out[name] = QJsonValue::fromVariant(value);
  }

  return out;
}

QJsonObject &operator>>
(QJsonObject &in, std::shared_ptr<Serializable> model)
{
  const QMetaObject *metaobject = model->metaObject();
  int count = metaobject->propertyCount();

  for (int i = 0; i < count; ++i) {
    QMetaProperty metaproperty = metaobject->property(i);
    const char *name = metaproperty.name();

    QVariant value;
    model->setProperty(name, in[name].toVariant());
  }

  return in;
}

}
