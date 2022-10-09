#ifndef PIPELINE_CREATEGROMACSMODEL_CONFIGURATION_H
#define PIPELINE_CREATEGROMACSMODEL_CONFIGURATION_H

#include "../../config/configuration.h"

namespace Pipeline { namespace CreateGromacsModel {

class Configuration
: public Config::Configuration
{
  Q_OBJECT
public:
  enum class WaterModel : int {
    None = 0,
    SPC,
    TIP3P,
    TIP4P,
    TIP5P,
  };
  Q_ENUM(WaterModel);

  enum class ForceField : int {
    None = 0,
    CHARMM27,
    OPLSAA,
  };
  Q_ENUM(ForceField);

  Configuration();
  Q_PROPERTY(WaterModel waterModel MEMBER waterModel NOTIFY waterModelChanged)
  Q_PROPERTY(ForceField forceField MEMBER forceField NOTIFY forceFieldChanged)

  QWidget* getUI() override;
  QString toString() override;

signals:
  void waterModelChanged(WaterModel);
  void forceFieldChanged(ForceField);

private:
  WaterModel waterModel = WaterModel::SPC;
  ForceField forceField = ForceField::CHARMM27;
};

QString toString(Configuration::WaterModel type);
QString toString(Configuration::ForceField type);

} }

#endif
