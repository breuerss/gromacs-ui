#include "temperaturecouplinggroup.h"
#include <QDebug>
#include <QMetaProperty>
#include <cstring>

namespace Model {

TemperatureCouplingGroup::TemperatureCouplingGroup()
{
  // Ugly hack to make Group/Algorithm serializable as an integer
  // in QVariant
  static bool registered = false;
  if (!registered)
  {
    qRegisterMetaType<Group>("TemperatureCouplingGroup::Group");
    qRegisterMetaTypeStreamOperators<int>("TemperatureCouplingGroup::Group");
    registered = true;
  }
}

QString toString(TemperatureCouplingGroup::Group group)
{
  const static QMap<TemperatureCouplingGroup::Group, QString> map = {
    { TemperatureCouplingGroup::Group::System, "System" },
    { TemperatureCouplingGroup::Group::Protein, "Protein" },
    { TemperatureCouplingGroup::Group::Water, "Water" },
  };

  return map[group];
}

}
