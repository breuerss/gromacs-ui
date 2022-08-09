#include "temperaturegroupconfigform.h"
#include "ui_temperaturegroupconfigform.h"

TemperatureGroupConfigForm::TemperatureGroupConfigForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TemperatureGroupConfigForm)
{
    ui->setupUi(this);
}

TemperatureGroupConfigForm::~TemperatureGroupConfigForm()
{
    delete ui;
}
