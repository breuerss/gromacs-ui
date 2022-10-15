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
    conns << connect(ui->removeTemperatureGroup, &QToolButton::clicked,
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
    conns << connectToComboBox(
      ui->temperatureCouplingGroup, model.get(), "group",
      &TemperatureCouplingGroup::groupChanged
      );
    conns << connectToSpinBox<QDoubleSpinBox>(
      ui->temperature, model, "temperature", &TemperatureCouplingGroup::temperatureChanged);
    conns << connectToSpinBox<QDoubleSpinBox>(
      ui->temperatureUpdateInterval, model, "updateInterval", &TemperatureCouplingGroup::updateIntervalChanged);
}

TemperatureGroupConfigForm::~TemperatureGroupConfigForm()
{
  for (auto conn: conns)
  {
    disconnect(conn);
  }
  delete ui;
}
