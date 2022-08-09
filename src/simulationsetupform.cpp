#include "simulationsetupform.h"
#include "ui_simulationsetupform.h"
#include "model/simulation.h"
#include "gromacsconfigfilegenerator.h"
#include "uiconnectionhelper.h"

SimulationSetupForm::SimulationSetupForm(
  std::shared_ptr<Model::Simulation> newStep, QWidget *parent)
  : QWidget(parent)
    , step(newStep)
    , ui(new Ui::SimulationSetupForm)
{
  ui->setupUi(this);

  using Model::Simulation;

  QList<QPair<QString, Simulation::Type>> typeOptions = {
    { toString(Simulation::Type::None), Simulation::Type::None },
    { toString(Simulation::Type::Minimisation), Simulation::Type::Minimisation },
    { toString(Simulation::Type::NVT), Simulation::Type::NVT },
    { toString(Simulation::Type::NPT), Simulation::Type::NPT },
    //{ toString(Simulation::Type::NVE), Simulation::Type::NVE },
  };

  setOptions<Simulation::Type>(ui->simulationType, typeOptions);
  connectToComboBox<Simulation::Type>(
    ui->simulationType,
    step,
    "simulationType",
    [this] (Simulation::Type type) {
      updateUiForSimulationType(type);
    });

  connectToComboBox<Simulation::Type>(
    ui->simulationType,
    step,
    "simulationType",
    [this] (Simulation::Type type) {
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

void SimulationSetupForm::updateUiForSimulationType(Model::Simulation::Type type)
{
  hideSettings();
  setAlgorithmsForType(type);
  enableAllSettings();
  using Model::Simulation;
  switch(type)
  {
    case Simulation::Type::Minimisation:
      ui->electrostaticsGroup->setVisible(true);
      ui->vanDerWaalsGroup->setVisible(true);
      ui->generalGroup->setVisible(true);
      ui->outputSettingsGroup->setVisible(true);
      ui->minimisationGroup->setVisible(true);
      ui->positionOutputFrequency->setEnabled(false);
      ui->velocityOutputFrequency->setEnabled(false);
      ui->forceOutputFrequency->setEnabled(false);
      break;
    case Simulation::Type::NVT:
      ui->electrostaticsGroup->setVisible(true);
      ui->vanDerWaalsGroup->setVisible(true);
      ui->generalGroup->setVisible(true);
      ui->outputSettingsGroup->setVisible(true);
      ui->temperatureGroup->setVisible(true);
      ui->pressureGroup->setVisible(true);
      ui->pressureGroup->setEnabled(false);
      break;
    case Simulation::Type::NPT:
      ui->electrostaticsGroup->setVisible(true);
      ui->vanDerWaalsGroup->setVisible(true);
      ui->generalGroup->setVisible(true);
      ui->outputSettingsGroup->setVisible(true);
      ui->temperatureGroup->setVisible(true);
      ui->pressureGroup->setVisible(true);
    case Simulation::Type::None:
      break;
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

void SimulationSetupForm::setAlgorithmsForType(Model::Simulation::Type type)
{
  QList<QPair<QString, QVariant>> map;
  int defaultIndex = 0;

  using Model::Simulation;
  switch(type)
  {
    case Simulation::Type::Minimisation:
      map = {
        { "Steepest Descent", "steep" },
        { "Conjugate Gradient", "cg" },
      };
      defaultIndex = 1;
      break;
    case Simulation::Type::NVT:
    case Simulation::Type::NPT:
    case Simulation::Type::NVE:
      map = {
        { "Leap Frog Integrator (md)", "md" },
        { "Stochastic Dynamics Integrator (sd)", "sd" },
      };
      defaultIndex = 1;
    default:
      break;
  }

  setOptions(ui->algorithm, map, defaultIndex);
}
