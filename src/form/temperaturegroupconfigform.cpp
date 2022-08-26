#include "temperaturegroupconfigform.h"
#include "ui_temperaturegroupconfigform.h"
#include "connectionhelper.h"
#include "../config/temperaturecouplinggroup.h"

TemperatureGroupConfigForm::TemperatureGroupConfigForm(
  std::shared_ptr<Config::TemperatureCouplingGroup> model, QWidget *parent)
    : QWidget(parent)
    , model(model)
    , ui(new Ui::TemperatureGroupConfigForm)
{
    ui->setupUi(this);
    connect(ui->removeTemperatureGroup, &QToolButton::clicked,
            this, &TemperatureGroupConfigForm::removeRequested);

    using CouplingGroup = Config::TemperatureCouplingGroup::Group;
    QList<QPair<QString, CouplingGroup>> options = {
      { "System",  CouplingGroup::System },
      { "Protein",  CouplingGroup::Protein },
      { "Water",  CouplingGroup::Water },
    };
    setOptions<CouplingGroup>(
      ui->temperatureCouplingGroup, options);
    connectToComboBox<CouplingGroup>(
      ui->temperatureCouplingGroup, model, "group");
    connectToSpinBox<QDoubleSpinBox, double>(
      ui->temperature, model, "temperature");
    connectToSpinBox<QDoubleSpinBox, double>(
      ui->temperatureUpdateInterval, model, "updateInterval");
}

TemperatureGroupConfigForm::~TemperatureGroupConfigForm()
{
    delete ui;
}
