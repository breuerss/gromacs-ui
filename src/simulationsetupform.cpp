#include "simulationsetupform.h"
#include "ui_simulationsetupform.h"
#include "model/simulationtype.h"
#include "model/step.h"
#include "gromacsconfigfilegenerator.h"
#include "uiconnectionhelper.h"

SimulationSetupForm::SimulationSetupForm(
  std::shared_ptr<Model::Step> newStep, QWidget *parent)
  : QWidget(parent)
    , step(newStep)
    , ui(new Ui::SimulationSetupForm)
{
  ui->setupUi(this);
  setOptions(ui->simulationType, {
    { "None", QVariant::fromValue(Model::SimulationType::None) },
      { "Minimisation", QVariant::fromValue(Model::SimulationType::Minimisation) },
      //        { "Microcanonical (NVE)", SimulationType::NVE },
      { "Canonical (NVT)", QVariant::fromValue(Model::SimulationType::NVT) },
      //        { "Gibbs/Isobaric-isothermal (NPT)", SimulationType::NPT },
  });

  connectToComboBox<Model::SimulationType>(
    ui->simulationType,
    step,
    "simulationType",
    [this] (Model::SimulationType type) {
      updateUiForSimulationType(type);
    });

  QWidget* container = ui->settingsWidget;
  connectToComboBox<QString>(container, step, "algorithm");

  connectToSpinBox<QSpinBox, int>(container, step, "numberOfSteps");
  connectToSpinBox<QDoubleSpinBox, double>(container, step, "minimisationStepSize");
  connectToSpinBox<QDoubleSpinBox, double>(container, step, "minimisationMaximumForce");
  connectToSpinBox<QSpinBox, int>(container, step, "energyOutputFrequency");
  connectToSpinBox<QSpinBox, int>(container, step, "positionOutputFrequency");
  connectToSpinBox<QSpinBox, int>(container, step, "velocityOutputFrequency");
  connectToSpinBox<QSpinBox, int>(container, step, "forceOutputFrequency");
  connectToSpinBox<QSpinBox, int>(ui->compressedPositionOutputFrequency, step, "compressedPositionOutputFrequency");
  connectToSpinBox<QSpinBox, int>(ui->logOutputFrequency, step, "logOutputFrequency");

  setOptions(ui->electrostaticAlgorithm, {
    {"None", "no"},
      {"PME", "PME"},
  }, 1);
  connectToComboBox<QString>(ui->electrostaticAlgorithm, step, "electrostaticAlgorithm");
  connectToSpinBox<QDoubleSpinBox, double>(ui->electrostaticCutoffRadius, step, "electrostaticCutoffRadius");
  connectToSpinBox<QDoubleSpinBox, double>(ui->fourierSpacing, step, "fourierSpacing");
  connectToSpinBox<QDoubleSpinBox, double>(ui->vdwCutoffRadius, step, "vdwCutoffRadius");
}

SimulationSetupForm::~SimulationSetupForm()
{
  delete ui;
}

void SimulationSetupForm::updateUiForSimulationType(Model::SimulationType type)
{
  hideSettings();
  setAlgorithmsForType(type);
  enableAllSettings();
  switch(type)
  {
    case Model::SimulationType::Minimisation:
      ui->electrostaticsGroup->setVisible(true);
      ui->vanDerWaalsGroup->setVisible(true);
      ui->generalGroup->setVisible(true);
      ui->outputSettingsGroup->setVisible(true);
      ui->minimisationGroup->setVisible(true);
      ui->positionOutputFrequency->setEnabled(false);
      ui->velocityOutputFrequency->setEnabled(false);
      ui->forceOutputFrequency->setEnabled(false);
      break;
    case Model::SimulationType::NVT:
      ui->electrostaticsGroup->setVisible(true);
      ui->vanDerWaalsGroup->setVisible(true);
      ui->generalGroup->setVisible(true);
      ui->outputSettingsGroup->setVisible(true);
      break;
    case Model::SimulationType::None:
    default:
      break;
  }
}

void SimulationSetupForm::hideSettings()
{
  ui->electrostaticsGroup->setVisible(false);
  ui->vanDerWaalsGroup->setVisible(false);
  ui->generalGroup->setVisible(false);
  ui->outputSettingsGroup->setVisible(false);
  ui->minimisationGroup->setVisible(false);
  ui->temperatureGroup->setVisible(false);
  ui->pressureGroup->setVisible(false);
}

void SimulationSetupForm::enableAllSettings()
{
  for (QWidget* child: ui->settingsWidget->findChildren<QWidget*>())
  {
    child->setEnabled(true);
  }
}

void SimulationSetupForm::setAlgorithmsForType(Model::SimulationType type)
{
  QList<QPair<QString, QVariant>> map;
  int defaultIndex = 0;

  switch(type)
  {
    case Model::SimulationType::Minimisation:
      map = QList<QPair<QString, QVariant>>({
        { "None", "" },
          { "Steepest Descent", "steep" },
          { "Conjugate Gradient", "cg" },
      });
      defaultIndex = 1;
      break;
    case Model::SimulationType::NVT:
      map = QList<QPair<QString, QVariant>>({
        { "None", "" },
          { "Leap Frog", "md" },
      });
      defaultIndex = 1;
    default:
      break;
  }

  setOptions(ui->algorithm, map, defaultIndex);
}
