#include "simulationsetupform.h"
#include "ui_simulationsetupform.h"
#include "temperaturegroupconfigform.h"
#include "../model/simulation.h"
#include "../gromacsconfigfilegenerator.h"
#include "connectionhelper.h"

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

  QList<QPair<QString, Simulation::PressureCouplingType>> pressureCouplingTypeOptions = {
    { "Isotropic", Simulation::PressureCouplingType::Isotropic },
    { "SemiIsoTropic", Simulation::PressureCouplingType::SemiIsoTropic },
    { "Anisotropic", Simulation::PressureCouplingType::Anisotropic },
    { "Surface Tension", Simulation::PressureCouplingType::SurfaceTension },
  };

  setOptions<Simulation::PressureCouplingType>(ui->pressureCouplingType, pressureCouplingTypeOptions);

  connectToComboBox<Simulation::PressureCouplingType>(
    ui->pressureCouplingType,
    step,
    "pressureCouplingType"
    );

  QWidget* container = ui->settingsWidget;
  connectToComboBox<Simulation::Algorithm>(container, step, "algorithm");

  connectToSpinBox<QSpinBox, int>(container, step, "numberOfSteps");
  connectToSpinBox<QDoubleSpinBox, double>(container, step, "minimisationStepSize");
  connectToSpinBox<QDoubleSpinBox, double>(container, step, "minimisationMaximumForce");
  connectToSpinBox<QSpinBox, int>(container, step, "energyOutputFrequency");
  connectToSpinBox<QSpinBox, int>(container, step, "positionOutputFrequency");
  connectToSpinBox<QSpinBox, int>(container, step, "velocityOutputFrequency");
  connectToSpinBox<QSpinBox, int>(container, step, "forceOutputFrequency");
  connectToSpinBox<QSpinBox, int>(ui->compressedPositionOutputFrequency, step, "compressedPositionOutputFrequency");
  connectToSpinBox<QSpinBox, int>(ui->logOutputFrequency, step, "logOutputFrequency");

  // pressure
  connectToComboBox<Model::Simulation::PressureAlgorithm>(
              ui->pressureAlgorithm, step, "pressureAlgorithm"
              );
  connectToSpinBox<QDoubleSpinBox, double>(ui->pressure, step, "pressure");
  connectToSpinBox<QDoubleSpinBox, double>(ui->pressureUpdateInterval, step, "pressureUpdateInterval");
  connectToComboBox<Model::Simulation::PressureCouplingType>(
              ui->pressureCouplingType, step, "pressureCouplingType"
              );

  // temperature
  connect(ui->addTemperatureCouplingGroup, &QToolButton::clicked, [this] () {
    step->addTemperatureCouplingGroup();
  });
  connectToComboBox<Model::Simulation::TemperatureAlgorithm>(
              ui->temperatureAlgorithm, step, "temperatureAlgorithm"
              );

  setOptions(ui->electrostaticAlgorithm, {
     {"None", "no"},
     {"PME", "PME"},
  }, 1);
  connectToComboBox<QString>(ui->electrostaticAlgorithm, step, "electrostaticAlgorithm");
  connectToSpinBox<QDoubleSpinBox, double>(ui->electrostaticCutoffRadius, step, "electrostaticCutoffRadius");
  connectToSpinBox<QDoubleSpinBox, double>(ui->fourierSpacing, step, "fourierSpacing");
  connectToSpinBox<QDoubleSpinBox, double>(ui->vdwCutoffRadius, step, "vdwCutoffRadius");
  connect(ui->createConfig, &QPushButton::clicked, [this] () {
    GromacsConfigFileGenerator::generate(step, "/tmp/tmp.mdp");
  });


  connect(step.get(), &Model::Simulation::temperatureCouplingGroupAdded,
          this, &SimulationSetupForm::addTemperatureCouplingGroup);
  connect(step.get(), &Model::Simulation::temperatureCouplingGroupRemoved,
          this, &SimulationSetupForm::removeTemperatureCouplingGroup);
  for (auto group : step->getTemperatureCouplingGroups())
  {
    addTemperatureCouplingGroup(group);
  }
}

SimulationSetupForm::~SimulationSetupForm()
{
  for (QMetaObject::Connection conn: conns) {
    QObject::disconnect(conn);
  }
  delete ui;
}

void SimulationSetupForm::updateUiForSimulationType(Model::Simulation::Type type)
{
  hideSettings();
  setAlgorithmsForType(type);
  setPressureAlgorithmsForType(type);
  setTemperatureAlgorithmsForType(type);
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
  for (QWidget* child: ui->settingsWidget->findChildren<QSpinBox*>())
  {
    child->setEnabled(true);
  }
  for (QWidget* child: ui->settingsWidget->findChildren<QGroupBox*>())
  {
    child->setEnabled(true);
  }
}

void SimulationSetupForm::setAlgorithmsForType(Model::Simulation::Type type)
{
  using Model::Simulation;
  QList<QPair<QString, Simulation::Algorithm>> map;
  int defaultIndex = 0;

  switch(type)
  {
    case Simulation::Type::Minimisation:
      map = {
        { "Steepest Descent", Simulation::Algorithm::SteepestDecent },
        { "Conjugate Gradient", Simulation::Algorithm::ConjugateGradient },
      };
      defaultIndex = 1;
      break;
    case Simulation::Type::NVT:
    case Simulation::Type::NPT:
    case Simulation::Type::NVE:
      map = {
        { "Leap Frog Integrator (md)", Simulation::Algorithm::LeapFrog },
        { "Stochastic Dynamics Integrator (sd)", Simulation::Algorithm::StochasticDynamics },
      };
      defaultIndex = 1;
    default:
      break;
  }

  setOptions<Simulation::Algorithm>(ui->algorithm, map, defaultIndex);
}

void SimulationSetupForm::setPressureAlgorithmsForType(Model::Simulation::Type type)
{
  using Model::Simulation;
  QList<QPair<QString, Simulation::PressureAlgorithm>> map({
    { "None", Simulation::PressureAlgorithm::None }
  });
  if (type == Model::Simulation::Type::NPT)
  {
    map = {
      { "Berendsen", Simulation::PressureAlgorithm::Berendsen },
      { "Parrinello-Rahman", Simulation::PressureAlgorithm::ParrinelloRahman }
    };
  }
  setOptions<Simulation::PressureAlgorithm>(ui->pressureAlgorithm, map);
}

void SimulationSetupForm::setTemperatureAlgorithmsForType(Model::Simulation::Type type)
{
  using Model::Simulation;
  QList<QPair<QString, Simulation::TemperatureAlgorithm>> map({
    { "None", Simulation::TemperatureAlgorithm::None }
  });
  if (type == Model::Simulation::Type::NPT || type == Model::Simulation::Type::NVT)
  {
    map = {
      { "Berendsen", Simulation::TemperatureAlgorithm::Berendsen },
      { "Nose-Hoover", Simulation::TemperatureAlgorithm::NoseHoover },
      { "Velocity Rescale", Simulation::TemperatureAlgorithm::VelocityRescale }
    };
  }
  setOptions<Simulation::TemperatureAlgorithm>(ui->temperatureAlgorithm, map);
}

void SimulationSetupForm::addTemperatureCouplingGroup(
  std::shared_ptr<Model::TemperatureCouplingGroup> couplingGroup, int at)
{
  using Model::TemperatureCouplingGroup;
  auto callback = [couplingGroup, this] (TemperatureCouplingGroup::Group groupType) {
    bool enabled = groupType != TemperatureCouplingGroup::Group::System;
    ui->addTemperatureCouplingGroup->setEnabled(enabled);
  };
  conns.push_back(connect(couplingGroup.get(), &TemperatureCouplingGroup::groupChanged, callback));

  auto groupType = couplingGroup->property("group").value<TemperatureCouplingGroup::Group>();
  callback(groupType);

  TemperatureGroupConfigForm* groupEditor = new TemperatureGroupConfigForm(couplingGroup);
  connect(groupEditor, &TemperatureGroupConfigForm::removeRequested, [this, groupEditor] () {
    int at = ui->temperatureCouplingGroups->indexOf(groupEditor);
    step->removeTemperatureCouplingGroup(at);
  });
  ui->temperatureCouplingGroups->insertWidget(at, groupEditor);
}

void SimulationSetupForm::removeTemperatureCouplingGroup(
  std::shared_ptr<Model::TemperatureCouplingGroup>, int at)
{
  ui->temperatureCouplingGroups->takeAt(at)->widget()->deleteLater();
}

