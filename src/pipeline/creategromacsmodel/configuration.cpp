#include "configuration.h"
#include "../ui/gromacssystemsettings.h"
#include <memory>
#include <QMetaProperty>

namespace Pipeline { namespace CreateGromacsModel {

Configuration::Configuration()
{
  static bool registered = false;
  if (!registered)
  {
    qRegisterMetaType<WaterModel>("Configuration::WaterModel");
    qRegisterMetaTypeStreamOperators<int>("Configuration::WaterModel");
    qRegisterMetaType<ForceField>("Configuration::ForceField");
    qRegisterMetaTypeStreamOperators<int>("Configuration::ForceField");
    registered = true;
  }

  connectAllSignals();
}

QWidget* Configuration::getUI()
{
  return new GromacsSystemSettings(this);
}

QString Configuration::toString()
{
  return tr("Water model") + ": " + CreateGromacsModel::toString(waterModel).toUpper() + "\n"
    + tr("Force field") + ": " + CreateGromacsModel::toString(forceField).toUpper();
}

QString toString(Configuration::WaterModel type)
{
  const static QMap<Configuration::WaterModel, QString> map = {
    { Configuration::WaterModel::SPC, "spc" },
    { Configuration::WaterModel::TIP3P, "tip3p" },
    { Configuration::WaterModel::TIP4P, "tip4p" },
    { Configuration::WaterModel::TIP5P, "tip5p" },
  };

  return map[type];
}

QString toString(Configuration::ForceField type)
{
  const static QMap<Configuration::ForceField, QString> map = {
    { Configuration::ForceField::CHARMM27, "charmm27" },
    { Configuration::ForceField::OPLSAA, "oplsaa" },
  };

  return map[type];
}


} }
