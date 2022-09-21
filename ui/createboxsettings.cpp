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
  connectToComboBox(ui->boxType, config, "boxType", &Pipeline::CreateBox::Configuration::boxTypeChanged);
  connectToSpinBox<QDoubleSpinBox, double>(ui->distanceToEdge, config, "distance");
}

CreateBoxSettings::~CreateBoxSettings()
{
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
