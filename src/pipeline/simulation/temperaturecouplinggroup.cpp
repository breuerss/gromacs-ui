#include "temperaturecouplinggroup.h"
#include "../../misc/bimaphelper.h"
#include <QDebug>
#include <QMetaProperty>
#include <cstring>

namespace Pipeline { namespace Simulation {

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

const static auto temperatureCouplingGroupsBimap =
makeBimap<TemperatureCouplingGroup::Group, QString>({
  { TemperatureCouplingGroup::Group::System, "System" },
  { TemperatureCouplingGroup::Group::Protein, "Protein" },
  { TemperatureCouplingGroup::Group::Water, "Water" },
});

QVariant temperatureGroupFrom(const QString& group)
{
  return QVariant::fromValue(temperatureCouplingGroupsBimap.right.at(group));
}

QString toString(TemperatureCouplingGroup::Group group)
{
  return temperatureCouplingGroupsBimap.left.at(group);
}

} }
