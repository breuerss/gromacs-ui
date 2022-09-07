#include "serializable.h"
#include <QMetaObject>
#include <QMetaProperty>

namespace Model {

QDataStream &operator<<(QDataStream &out, const std::shared_ptr<Serializable> model)
{
  out << *(model.get());

  return out;
}

QDataStream &operator>>(QDataStream &in, std::shared_ptr<Serializable> model)
{
  in >> *(model.get());

  return in;
}

QDataStream &operator<<(QDataStream &out, const Serializable& model)
{
  const QMetaObject *metaobject = model.metaObject();
  int count = metaobject->propertyCount();
  for (int i = 0; i < count; ++i) {
    QMetaProperty metaproperty = metaobject->property(i);
    const char *name = metaproperty.name();
    QVariant value = model.property(name);

    out << value;
  }

  return out;
}

QDataStream &operator>>(QDataStream &in, Serializable &model)
{
  const QMetaObject *metaobject = model.metaObject();
  int count = metaobject->propertyCount();

  for (int i = 0; i < count; ++i) {
    QMetaProperty metaproperty = metaobject->property(i);
    const char *name = metaproperty.name();

    QVariant value;
    in >> value;
    model.setProperty(name, value);
  }

  return in;
}

}
