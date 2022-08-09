#include "temperaturegroupconfigform.h"
#include "ui_temperaturegroupconfigform.h"
#include "connectionhelper.h"
#include "../model/temperaturecouplinggroup.h"

TemperatureGroupConfigForm::TemperatureGroupConfigForm(
  std::shared_ptr<Model::TemperatureCouplingGroup> model, QWidget *parent)
    : QWidget(parent)
    , model(model)
    , ui(new Ui::TemperatureGroupConfigForm)
{
    ui->setupUi(this);
    connect(ui->removeTemperatureGroup, &QToolButton::clicked,
            this, &TemperatureGroupConfigForm::removeRequested);

    QList<QPair<QString, Model::TemperatureCouplingGroup::Group>> options = {
      { "System",  Model::TemperatureCouplingGroup::Group::System },
      { "Protein",  Model::TemperatureCouplingGroup::Group::Protein },
      { "Water",  Model::TemperatureCouplingGroup::Group::Water },
    };
    setOptions<Model::TemperatureCouplingGroup::Group>(
      ui->temperatureCouplingGroup, options);
    connectToComboBox<Model::TemperatureCouplingGroup::Group>(
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
