#include "gromacssystemsettings.h"
#include "ui_gromacssystemsettings.h"
#include "connectionhelper.h"

GromacsSystemSettings::GromacsSystemSettings(
  Pipeline::CreateGromacsModel::Configuration* newConfig
  )
  : QWidget(nullptr)
    , ui(new Ui::GromacsSystemSettings)
    , config(newConfig)
{
    ui->setupUi(this);
    prepareWaterOptions();
    prepareForceFieldOptions();
    connectToComboBox<QString>(ui->waterModel, config, "waterModel");
    connectToComboBox<QString>(ui->forceField, config, "forceField");
}

GromacsSystemSettings::~GromacsSystemSettings()
{
    delete ui;
}

void GromacsSystemSettings::prepareWaterOptions()
{
  using Pipeline::CreateGromacsModel::Configuration;
  setOptions<Configuration::WaterModel>(
    ui->waterModel,
    {
      //{ "None", SystemSetup::WaterModel::None }, // not yet properly handled
      { "SPC", Configuration::WaterModel::SPC },
      { "TIP3P", Configuration::WaterModel::TIP3P },
      { "TIP4P", Configuration::WaterModel::TIP4P },
      { "TIP5P", Configuration::WaterModel::TIP5P },
    }, Configuration::WaterModel::SPC);
}

void GromacsSystemSettings::prepareForceFieldOptions()
{
  using Pipeline::CreateGromacsModel::Configuration;
  setOptions<Configuration::ForceField>(
    ui->forceField,
    {
      { "CHARMM27", Configuration::ForceField::CHARMM27 },
      { "OPLS-AA/L", Configuration::ForceField::OPLSAA }
  }, Configuration::ForceField::CHARMM27);
}


