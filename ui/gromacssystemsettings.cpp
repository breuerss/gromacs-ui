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
    using Pipeline::CreateGromacsModel::Configuration;
    connectToComboBox(ui->waterModel, config, "waterModel", &Configuration::waterModelChanged);
    connectToComboBox(ui->forceField, config, "forceField", &Configuration::forceFieldChanged);
}

GromacsSystemSettings::~GromacsSystemSettings()
{
    delete ui;
}

void GromacsSystemSettings::prepareWaterOptions()
{
  using WaterModel = Pipeline::CreateGromacsModel::Configuration::WaterModel;
  setOptions<WaterModel>(
    ui->waterModel,
    {
      //{ "None", SystemSetup::WaterModel::None }, // not yet properly handled
      { "SPC", WaterModel::SPC },
      { "TIP3P", WaterModel::TIP3P },
      { "TIP4P", WaterModel::TIP4P },
      { "TIP5P", WaterModel::TIP5P },
    }, WaterModel::SPC);
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


