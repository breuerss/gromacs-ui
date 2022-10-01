#include "timecontrolsettings.h"
#include "connectionhelper.h"
#include "ui_timecontrolsettings.h"
#include <qcombobox.h>

TimeControlSettings::TimeControlSettings(Pipeline::TimeStepControl::Configuration* newConfig) :
    QWidget(nullptr)
    , ui(new Ui::TimeControlSettings)
    , config(newConfig)
{
    ui->setupUi(this);
    setupTimeUnitOptions();
    using Pipeline::TimeStepControl::Configuration;
    conns << connectToComboBox(ui->timeUnit, config, "timeUnit", &Configuration::timeUnitChanged);
    conns << connectToSpinBox<QDoubleSpinBox>(
      ui->startTimeStep, config,
      "startTimeStep", &Configuration::startTimeStepChanged
      );
    conns << connectToSpinBox<QDoubleSpinBox>(
      ui->endTimeStep, config,
      "endTimeStep", &Configuration::endTimeStepChanged
    );
    conns << connectToSpinBox<QDoubleSpinBox>(
      ui->interval, config,
      "interval", &Configuration::intervalChanged
      );
    conns << connectToSpinBox<QSpinBox>(
      ui->stride, config,
      "stride", &Configuration::strideChanged
      );

    auto setUnitLabel = [this] (const QString& timeUnit) {
      ui->endTimeStepUnitLabel->setText(timeUnit);
      ui->startTimeStepUnitLabel->setText(timeUnit);
      ui->intervalUnitLabel->setText(timeUnit);
    };
    conns << connect(ui->timeUnit, &QComboBox::currentTextChanged, setUnitLabel);
    setUnitLabel(ui->timeUnit->currentText());
}

TimeControlSettings::~TimeControlSettings()
{
  for (auto conn : conns)
  {
    disconnect(conn);
  }
  delete ui;
}

void TimeControlSettings::setupTimeUnitOptions()
{
  using TimeUnit = Pipeline::TimeStepControl::Configuration::TimeUnit;
  setOptions<TimeUnit>(
    ui->timeUnit,
    {
      { "fs", TimeUnit::FemtoSeconds },
      { "ns", TimeUnit::NanosSconds },
      { "ps", TimeUnit::PicoSeconds },
      { "us", TimeUnit::MicroSeconds },
      { "ms", TimeUnit::MilliSeconds },
      { "s", TimeUnit::Seconds },
    }, TimeUnit::PicoSeconds);
}
