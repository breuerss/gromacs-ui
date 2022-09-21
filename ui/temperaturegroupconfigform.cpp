#include "temperaturegroupconfigform.h"
#include "ui_temperaturegroupconfigform.h"
#include "connectionhelper.h"
#include "../src/pipeline/simulation/temperaturecouplinggroup.h"

TemperatureGroupConfigForm::TemperatureGroupConfigForm(
  std::shared_ptr<Pipeline::Simulation::TemperatureCouplingGroup> model, QWidget *parent)
    : QWidget(parent)
    , model(model)
    , ui(new Ui::TemperatureGroupConfigForm)
{
    ui->setupUi(this);
    connect(ui->removeTemperatureGroup, &QToolButton::clicked,
            this, &TemperatureGroupConfigForm::removeRequested);

    using TemperatureCouplingGroup = Pipeline::Simulation::TemperatureCouplingGroup;
    using CouplingGroup = TemperatureCouplingGroup::Group;
    QList<QPair<QString, CouplingGroup>> options = {
      { "System",  CouplingGroup::System },
      { "Protein",  CouplingGroup::Protein },
      { "Water",  CouplingGroup::Water },
    };
    setOptions<CouplingGroup>(
      ui->temperatureCouplingGroup, options);
    connectToComboBox(
      ui->temperatureCouplingGroup, model.get(), "group",
      &TemperatureCouplingGroup::groupChanged
      );
    connectToSpinBox<QDoubleSpinBox, double>(
      ui->temperature, model, "temperature");
    connectToSpinBox<QDoubleSpinBox, double>(
      ui->temperatureUpdateInterval, model, "updateInterval");
}

TemperatureGroupConfigForm::~TemperatureGroupConfigForm()
{
    delete ui;
}
