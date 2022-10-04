#include "createboxsettings.h"
#include "ui_createboxsettings.h"
#include "connectionhelper.h"

CreateBoxSettings::CreateBoxSettings(Pipeline::CreateBox::Configuration* newConfig)
  : QWidget(nullptr)
    , ui(new Ui::CreateBoxSettings)
    , config(newConfig)
{
  ui->setupUi(this);
  prepareBoxOptions();
  using Pipeline::CreateBox::Configuration;
  conns << connectToComboBox(ui->boxType, config, "boxType", &Configuration::boxTypeChanged);
  conns << connectToSpinBox<QDoubleSpinBox>(ui->distanceToEdge, config, "distance", &Configuration::distanceChanged);
}

CreateBoxSettings::~CreateBoxSettings()
{
  for (auto conn : conns)
  {
    disconnect(conn);
  }
  delete ui;
}

void CreateBoxSettings::prepareBoxOptions()
{
  using BoxType = Pipeline::CreateBox::Configuration::BoxType;
  setOptions<BoxType>(
    ui->boxType,
    {
      { "Cubic", BoxType::Cubic },
      { "Octahedron", BoxType::Octahedron },
      { "Dodecahedron", BoxType::Dodecahedron }
    },
    BoxType::Dodecahedron);
}
