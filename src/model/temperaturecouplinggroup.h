#ifndef TEMPERATURECOUPLINGGROUP_H
#define TEMPERATURECOUPLINGGROUP_H

#include "serializable.h"
#include <QString>
#include <QDataStream>

namespace Model {

class TemperatureCouplingGroup : public Serializable
{
  Q_OBJECT
public:
  enum class Group : int {
    System = 0,
    Protein,
    Water,
  };
  Q_ENUM(Group);

  TemperatureCouplingGroup();

  Q_PROPERTY(Group group MEMBER group NOTIFY groupChanged);

  Q_PROPERTY(double temperature MEMBER temperature NOTIFY temperatureChanged);
  Q_PROPERTY(double updateInterval MEMBER updateInterval NOTIFY updateIntervalChanged);

signals:
  void groupChanged(Group);

  void temperatureChanged(double);
  void updateIntervalChanged(double);

private:
  Group group = Group::System;

  double temperature = 300.0;
  double updateInterval = 1.00;
};

QString toString(TemperatureCouplingGroup::Group group);

}

#endif
