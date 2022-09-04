#ifndef PIPELINE_CREATEGROMACSMODEL_CONFIGURATION_H
#define PIPELINE_CREATEGROMACSMODEL_CONFIGURATION_H

#include "../../model/serializable.h"
#include "../../config/configuration.h"
#include <QString>
#include <memory>

namespace Pipeline { namespace CreateGromacsModel {

class Configuration
: public Model::Serializable
, public Config::Configuration
, public std::enable_shared_from_this<Configuration>
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

signals:
  void waterModelChanged(const WaterModel&);
  void forceFieldChanged(const ForceField&);

private:
  WaterModel waterModel = WaterModel::SPC;
  ForceField forceField = ForceField::CHARMM27;
};

QString toString(Configuration::WaterModel type);
QString toString(Configuration::ForceField type);

} }

#endif
