#include "simulationsetupform.h"
#include "progresschart.h"
#include "ui_simulationsetupform.h"
#include "temperaturegroupconfigform.h"
#include "connectionhelper.h"

#include "../src/pipeline/simulation/configuration.h"
#include "../src/gromacsconfigfilegenerator.h"
#include "../src/simulationstatuschecker.h"
#include "../src/appprovider.h"

#include <algorithm>
#include <QDateTime>

SimulationSetupForm::SimulationSetupForm(
  Pipeline::Simulation::Configuration* newConfiguration
  )
  : QWidget(nullptr)
  , ui(new Ui::SimulationSetupForm)
  , configuration(newConfiguration)
{
  ui->setupUi(this);

  conns << connectToCheckbox(ui->resume, configuration, "resume");

  using Type = Pipeline::Simulation::Configuration::Type;

  connect(
    configuration, &Pipeline::Simulation::Configuration::simulationTypeChanged,
    [this] (Type type) {
      updateUiForSimulationType(type);
      qDebug() << "simulationtype changed";
    });
  updateUiForSimulationType(configuration->property("simulationType").value<Type>());

  QList<QPair<QString, Type>> typeOptions = {
    { toString(Type::None), Type::None },
    { toString(Type::Minimisation), Type::Minimisation },
    { toString(Type::NVT), Type::NVT },
    { toString(Type::NPT), Type::NPT },
    //{ toString(Configuration::Type::NVE), Configuration::Type::NVE },
  };

  using PressureCouplingType = Pipeline::Simulation::Configuration::PressureCouplingType;
  QList<QPair<QString, PressureCouplingType>> pressureCouplingTypeOptions = {
    { "Isotropic", PressureCouplingType::Isotropic },
    { "SemiIsoTropic", PressureCouplingType::SemiIsoTropic },
    { "Anisotropic", PressureCouplingType::Anisotropic },
    { "Surface Tension", PressureCouplingType::SurfaceTension },
  };

  setOptions<PressureCouplingType>(ui->pressureCouplingType, pressureCouplingTypeOptions);

  conns << connectToComboBox<PressureCouplingType, Configuration>(
    ui->pressureCouplingType,
    configuration,
    "pressureCouplingType",
    &Configuration::pressureCouplingTypeChanged
    );

  using Simulation = Pipeline::Simulation::Configuration;
  QWidget* container = ui->settingsWidget;
  conns << connectToComboBox<Simulation::Algorithm>(container, configuration, "algorithm", &Configuration::algorithmChanged);

  conns << connectToSpinBox<QDoubleSpinBox>(ui->numberOfSteps, configuration, "numberOfSteps", &Simulation::numberOfStepsChanged);
  conns << connectToSpinBox<QDoubleSpinBox>(ui->timeStep, configuration, "timeStep", &Simulation::timeStepChanged);
  conns << connectToSpinBox<QDoubleSpinBox>(container, configuration, "minimisationStepSize", &Simulation::minimisationStepSizeChanged);
  conns << connectToSpinBox<QDoubleSpinBox>(container, configuration, "minimisationMaximumForce", &Simulation::minimisationMaximumForceChanged);
  conns << connectToSpinBox<QSpinBox>(container, configuration, "energyOutputFrequency", &Simulation::energyOutputFrequencyChanged);
  conns << connectToSpinBox<QSpinBox>(container, configuration, "positionOutputFrequency", &Simulation::positionOutputFrequencyChanged);
  conns << connectToSpinBox<QSpinBox>(container, configuration, "velocityOutputFrequency", &Simulation::velocityOutputFrequencyChanged);
  conns << connectToSpinBox<QSpinBox>(container, configuration, "forceOutputFrequency", &Simulation::forceOutputFrequencyChanged);
  conns << connectToSpinBox<QSpinBox>(ui->compressedPositionOutputFrequency, configuration, "compressedPositionOutputFrequency", &Simulation::compressedPositionOutputFrequencyChanged);
  conns << connectToSpinBox<QSpinBox>(ui->logOutputFrequency, configuration, "logOutputFrequency", &Simulation::logOutputFrequencyChanged);

  // pressure
  conns << connectToComboBox<Simulation::PressureAlgorithm>(
              ui->pressureAlgorithm, configuration, "pressureAlgorithm", &Configuration::pressureAlgorithmChanged
              );
  conns << connectToSpinBox<QDoubleSpinBox>(ui->pressure, configuration, "pressure", &Simulation::pressureChanged);
  conns << connectToSpinBox<QDoubleSpinBox>(ui->pressureUpdateInterval, configuration, "pressureUpdateInterval", &Simulation::pressureUpdateIntervalChanged);

  // temperature
  conns << connect(ui->addTemperatureCouplingGroup, &QToolButton::clicked, [this] () {
    configuration->addTemperatureCouplingGroup();
  });
  conns << connectToComboBox<Simulation::TemperatureAlgorithm>(
    ui->temperatureAlgorithm, configuration, "temperatureAlgorithm", &Configuration::temperatureAlgorithmChanged
    );

  // electrostatics
  setOptions<Simulation::ElectrostaticAlgorithm>(ui->electrostaticAlgorithm, {
     {"PME", Simulation::ElectrostaticAlgorithm::PME },
     {"Cut-Off", Simulation::ElectrostaticAlgorithm::CutOff },
     {"Ewald", Simulation::ElectrostaticAlgorithm::Ewald },
     {"P3M-AD", Simulation::ElectrostaticAlgorithm::P3MAD },
     {"Reaction-Field", Simulation::ElectrostaticAlgorithm::ReactionField },
  }, Simulation::ElectrostaticAlgorithm::PME);
  conns << connectToComboBox<Simulation::ElectrostaticAlgorithm>(
    ui->electrostaticAlgorithm, configuration, "electrostaticAlgorithm",
    [this] (Simulation::ElectrostaticAlgorithm algorithm) {
      const bool needsPme = algorithm == Simulation::ElectrostaticAlgorithm::PME ||
        algorithm == Simulation::ElectrostaticAlgorithm::P3MAD ||
        algorithm == Simulation::ElectrostaticAlgorithm::Ewald;
      ui->electrostaticEwaldRtol->setEnabled(needsPme);
      ui->electrostaticEwaldRtolLabel->setEnabled(needsPme);
    }, &Configuration::electrostaticAlgorithmChanged
    );
  conns << connectToSpinBox<QDoubleSpinBox>(ui->electrostaticCutoffRadius, configuration, "electrostaticCutoffRadius", &Simulation::electrostaticCutoffRadiusChanged);
  conns << connectToSpinBox<QDoubleSpinBox>(ui->electrostaticEwaldRtol, configuration, "electrostaticEwaldRtol", &Simulation::electrostaticEwaldRtolChanged);


  // vdw
  setOptions<Simulation::VdwAlgorithm>(ui->vdwAlgorithm, {
     {"PME", Simulation::VdwAlgorithm::PME },
     {"Cut-Off", Simulation::VdwAlgorithm::CutOff },
  }, Simulation::VdwAlgorithm::PME);
  conns << connectToComboBox<Simulation::VdwAlgorithm>(
    ui->vdwAlgorithm, configuration, "vdwAlgorithm",
    [this] (Simulation::VdwAlgorithm algorithm) {
      // shoule be based on model
      const bool modfierAllowed = algorithm == Simulation::VdwAlgorithm::CutOff;
      ui->vdwModifierLabel->setEnabled(modfierAllowed);
      ui->vdwModifier->setEnabled(modfierAllowed);
      const bool needsPme = algorithm == Simulation::VdwAlgorithm::PME;
      ui->vdwEwaldRtol->setEnabled(needsPme);
      ui->vdwEwaldRtolLabel->setEnabled(needsPme);
    }, &Configuration::vdwAlgorithmChanged);
  conns << connectToSpinBox<QDoubleSpinBox>(ui->vdwEwaldRtol, configuration, "vdwEwaldRtol", &Simulation::vdwEwaldRtolChanged);
  setOptions<Simulation::VdwModifier>(ui->vdwModifier, {
     {"None", Simulation::VdwModifier::None },
     {"Potential-Switch", Simulation::VdwModifier::PotentialSwitch },
     {"Potential-Shift", Simulation::VdwModifier::PotentialShift },
     {"Force-Switch", Simulation::VdwModifier::ForceSwitch },
  }, Simulation::VdwModifier::None);
  conns << connectToComboBox<Simulation::VdwModifier>(
    ui->vdwModifier, configuration, "vdwModifier",
    [this] (Simulation::VdwModifier modifier) {
      const bool switchRadiusNeeded = modifier == Simulation::VdwModifier::PotentialSwitch ||
        modifier == Simulation::VdwModifier::ForceSwitch;
      ui->vdwSwitchRadius->setEnabled(switchRadiusNeeded);
      ui->vdwSwitchRadiusLabel->setEnabled(switchRadiusNeeded);
    }, &Configuration::vdwModifierChanged);
  conns << connectToSpinBox<QDoubleSpinBox>(
    ui->vdwSwitchRadius, configuration, "vdwSwitchRadius", &Simulation::vdwSwitchRadiusChanged);
  conns << connectToSpinBox<QDoubleSpinBox>(
    ui->vdwCutoffRadius, configuration, "vdwCutoffRadius", &Simulation::vdwCutoffRadiusChanged);

  // PME
  conns << connect(configuration, &Simulation::pmeSettingsNeededChanged,
          [this] () {
            ui->pmeSettingsGroup->setEnabled(configuration->pmeSettingsNeeded());
          });
  conns << connectToSpinBox<QDoubleSpinBox>(ui->fourierSpacing, configuration, "fourierSpacing", &Simulation::fourierSpacingChanged);
  conns << connectToSpinBox<QSpinBox>(ui->pmeOrder, configuration, "pmeOrder", &Simulation::pmeOrderChanged);

  conns << connect(configuration, &Simulation::temperatureCouplingGroupAdded,
          this, &SimulationSetupForm::addTemperatureCouplingGroup);
  conns << connect(configuration, &Simulation::temperatureCouplingGroupRemoved,
          this, &SimulationSetupForm::removeTemperatureCouplingGroup);
  for (auto group : configuration->getTemperatureCouplingGroups())
  {
    addTemperatureCouplingGroup(group);
  }

  auto updateTimeStep = [this] (Simulation::Algorithm algorithm) {
    bool timeStepSupported = algorithm == Simulation::Algorithm::LeapFrog ||
      algorithm == Simulation::Algorithm::StochasticDynamics;
    ui->timeStep->setEnabled(timeStepSupported);
  };
  conns << connect(configuration, &Simulation::algorithmChanged, updateTimeStep);
  updateTimeStep(configuration->property("algorithm").value<Simulation::Algorithm>());

  auto updateDuration = [this] () {
    const double numberOfSteps = configuration->property("numberOfSteps").value<double>();
    if (numberOfSteps < 1)
    {
      return;
    }

    const double timeStep = configuration->property("timeStep").value<double>();

    // duration in fs
    double duration = timeStep * numberOfSteps;
    const int orderOfMagnitude = log10(duration);
    int unitStep = orderOfMagnitude / 3;
    const static std::map<int, QString> unitMap = {
      { 0, "fs" },
      { 1, "ps" },
      { 2, "ns" },
      { 3, "µs" },
      { 4, "ms" },
      { 5, "s" },
    };

    unitStep = std::min(unitStep, 5);
    duration /= pow(10, 3 * unitStep);

    ui->duration->setText(QString::number(duration) + " " + unitMap.at(unitStep));
  };
  conns << connect(configuration, &Simulation::numberOfStepsChanged, updateDuration);
  conns << connect(configuration, &Simulation::timeStepChanged, updateDuration);
  updateDuration();

}

SimulationSetupForm::~SimulationSetupForm()
{
  for (QMetaObject::Connection conn: conns) {
    QObject::disconnect(conn);
  }
  delete ui;
}

void SimulationSetupForm::updateUiForSimulationType(Pipeline::Simulation::Configuration::Type type)
{
  hideSettings();
  setAlgorithmsForType(type);
  setPressureAlgorithmsForType(type);
  setTemperatureAlgorithmsForType(type);
  enableAllSettings();
  using Simulation = Pipeline::Simulation::Configuration;
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

void SimulationSetupForm::setAlgorithmsForType(Pipeline::Simulation::Configuration::Type type)
{
  using Simulation = Pipeline::Simulation::Configuration;
  QList<QPair<QString, Simulation::Algorithm>> map;
  int defaultIndex = 0;

  switch(type)
  {
    case Simulation::Type::Minimisation:
      map = {
        { "Steepest Descent", Simulation::Algorithm::SteepestDescent },
        { "Conjugate Gradient", Simulation::Algorithm::ConjugateGradient },
      };
      break;
    case Simulation::Type::NVT:
    case Simulation::Type::NPT:
    case Simulation::Type::NVE:
      map = {
        { "Leap Frog Integrator (md)", Simulation::Algorithm::LeapFrog },
        { "Stochastic Dynamics Integrator (sd)", Simulation::Algorithm::StochasticDynamics },
      };
    default:
      break;
  }

  setOptions<Simulation::Algorithm>(ui->algorithm, map, defaultIndex);
}

void SimulationSetupForm::setPressureAlgorithmsForType(Pipeline::Simulation::Configuration::Type type)
{
  using Simulation = Pipeline::Simulation::Configuration;
  QList<QPair<QString, Simulation::PressureAlgorithm>> map({
    { "None", Simulation::PressureAlgorithm::None }
  });
  Simulation::PressureAlgorithm defaultValue = Simulation::PressureAlgorithm::None;
  if (type == Simulation::Type::NPT)
  {
    map = {
      { "Bussi", Simulation::PressureAlgorithm::Bussi },
      { "Berendsen", Simulation::PressureAlgorithm::Berendsen },
      { "Parrinello-Rahman", Simulation::PressureAlgorithm::ParrinelloRahman }
    };
    defaultValue = Simulation::PressureAlgorithm::Bussi;
  }
  setOptions<Simulation::PressureAlgorithm>(ui->pressureAlgorithm, map, defaultValue);
}

void SimulationSetupForm::setTemperatureAlgorithmsForType(Pipeline::Simulation::Configuration::Type type)
{
  using Simulation = Pipeline::Simulation::Configuration;
  QList<QPair<QString, Simulation::TemperatureAlgorithm>> map({
    { "None", Simulation::TemperatureAlgorithm::None }
  });
  Simulation::TemperatureAlgorithm defaultValue = Simulation::TemperatureAlgorithm::None;
  if (type == Simulation::Type::NPT || type == Simulation::Type::NVT)
  {
    map = {
      { "Velocity Rescale", Simulation::TemperatureAlgorithm::VelocityRescale },
      { "Berendsen", Simulation::TemperatureAlgorithm::Berendsen },
      { "Nose-Hoover", Simulation::TemperatureAlgorithm::NoseHoover },
    };
    defaultValue = Simulation::TemperatureAlgorithm::VelocityRescale;
  }
  setOptions<Simulation::TemperatureAlgorithm>(ui->temperatureAlgorithm, map, defaultValue);
}

void SimulationSetupForm::addTemperatureCouplingGroup(
  std::shared_ptr<Pipeline::Simulation::TemperatureCouplingGroup> couplingGroup, int at)
{
  using Pipeline::Simulation::TemperatureCouplingGroup;
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
    configuration->removeTemperatureCouplingGroup(at);
  });
  ui->temperatureCouplingGroups->insertWidget(at, groupEditor);
}

void SimulationSetupForm::removeTemperatureCouplingGroup(
  std::shared_ptr<Pipeline::Simulation::TemperatureCouplingGroup> group, int at)
{
  using CouplingGroup = Pipeline::Simulation::TemperatureCouplingGroup::Group;
  ui->temperatureCouplingGroups->takeAt(at)->widget()->deleteLater();
  if (!ui->temperatureCouplingGroups->count() ||
      group->property("group").value<CouplingGroup>() == CouplingGroup::System)

  {
    ui->addTemperatureCouplingGroup->setEnabled(true);
  }
}


