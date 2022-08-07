#include "step.h"
#include "simulationtype.h"
#include <QDebug>
#include <QMetaProperty>
#include <cstring>

namespace Model {

Step::Step()
{
    // Ugly hack to make SimulationType serializable as an integer
    // in QVariant
    static bool registered = false;
    if (!registered)
    {
      qRegisterMetaType<SimulationType>("Model::SimulationType");
      qRegisterMetaTypeStreamOperators<int>("Model::SimulationType");
      registered = true;
    }
}

QString Step::getName() const
{
  // TODO translation
  QString name("None");
  if (simulationType == SimulationType::Minimisation)
  {
    name = "Minimisation";
  }
  else if (simulationType == SimulationType::NVT)
  {
      name = "NVT";
  }


  return name;
}

QString Step::getDirectory() const
{
  // TODO update on simulationType change
  // TODO translation
  QString directory("None");
  if (simulationType == SimulationType::Minimisation)
  {
    directory = "min";
  }
  else if (simulationType == SimulationType::NVT)
  {
      directory = "nvt";
  }


  return directory;
}

QDataStream &operator<<(QDataStream &out, const Step& step)
{
    const QMetaObject *metaobject = step.metaObject();
    int count = metaobject->propertyCount();
    qDebug() << "Writing" << count << "properties from step";
    for (int i = 0; i < count; ++i) {
        QMetaProperty metaproperty = metaobject->property(i);
        const char *name = metaproperty.name();
        QVariant value = step.property(name);

        out << value;
    }

    return out;
}

QDataStream &operator>>(QDataStream &in, Step& step)
{
    const QMetaObject *metaobject = step.metaObject();
    int count = metaobject->propertyCount();

    qDebug() << "Reading" << count << "properties to step";
    for (int i = 0; i < count; ++i) {
        QMetaProperty metaproperty = metaobject->property(i);
        const char *name = metaproperty.name();

        QVariant value;
        in >> value;
        step.setProperty(name, value);
    }

    return in;
}

}
