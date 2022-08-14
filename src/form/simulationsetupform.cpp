#include "simulationsetupform.h"
#include "qframe.h"
#include "qnamespace.h"
#include "qpushbutton.h"
#include "qsizepolicy.h"
#include "src/command/runsimulation.h"
#include "ui_simulationsetupform.h"
#include "temperaturegroupconfigform.h"
#include "../model/simulation.h"
#include "../model/project.h"
#include "../gromacsconfigfilegenerator.h"
#include "connectionhelper.h"
#include "../simulationstatuschecker.h"
#include "../filecontentviewer.h"
#include <algorithm>
#include <QGraphicsView>
#include <QGraphicsLayout>
#include <QDateTime>

SimulationSetupForm::SimulationSetupForm(
  std::shared_ptr<Model::Project> newProject,
  std::shared_ptr<Model::Simulation> newSimulation,
  std::shared_ptr<Command::RunSimulation> newCommand,
  QWidget *parent
  )
  : QWidget(parent)
  , project(newProject)
  , simulation(newSimulation)
  , command(newCommand)
  , ui(new Ui::SimulationSetupForm)
{
  ui->setupUi(this);
  setupProgressValueChart();

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
    simulation,
    "simulationType",
    [this] (Simulation::Type type) {
      updateUiForSimulationType(type);
    });
  updateUiForSimulationType(simulation->property("simulationType").value<Simulation::Type>());

  QList<QPair<QString, Simulation::PressureCouplingType>> pressureCouplingTypeOptions = {
    { "Isotropic", Simulation::PressureCouplingType::Isotropic },
    { "SemiIsoTropic", Simulation::PressureCouplingType::SemiIsoTropic },
    { "Anisotropic", Simulation::PressureCouplingType::Anisotropic },
    { "Surface Tension", Simulation::PressureCouplingType::SurfaceTension },
  };

  setOptions<Simulation::PressureCouplingType>(ui->pressureCouplingType, pressureCouplingTypeOptions);

  connectToComboBox<Simulation::PressureCouplingType>(
    ui->pressureCouplingType,
    simulation,
    "pressureCouplingType"
    );

  QWidget* container = ui->settingsWidget;
  connectToComboBox<Simulation::Algorithm>(container, simulation, "algorithm");

  connectToSpinBox<QDoubleSpinBox, double>(ui->numberOfSteps, simulation, "numberOfSteps");
  connectToSpinBox<QDoubleSpinBox, double>(ui->timeStep, simulation, "timeStep");
  connectToSpinBox<QDoubleSpinBox, double>(container, simulation, "minimisationStepSize");
  connectToSpinBox<QDoubleSpinBox, double>(container, simulation, "minimisationMaximumForce");
  connectToSpinBox<QSpinBox, int>(container, simulation, "energyOutputFrequency");
  connectToSpinBox<QSpinBox, int>(container, simulation, "positionOutputFrequency");
  connectToSpinBox<QSpinBox, int>(container, simulation, "velocityOutputFrequency");
  connectToSpinBox<QSpinBox, int>(container, simulation, "forceOutputFrequency");
  connectToSpinBox<QSpinBox, int>(ui->compressedPositionOutputFrequency, simulation, "compressedPositionOutputFrequency");
  connectToSpinBox<QSpinBox, int>(ui->logOutputFrequency, simulation, "logOutputFrequency");

  // pressure
  connectToComboBox<Model::Simulation::PressureAlgorithm>(
              ui->pressureAlgorithm, simulation, "pressureAlgorithm"
              );
  connectToSpinBox<QDoubleSpinBox, double>(ui->pressure, simulation, "pressure");
  connectToSpinBox<QDoubleSpinBox, double>(ui->pressureUpdateInterval, simulation, "pressureUpdateInterval");
  connectToComboBox<Model::Simulation::PressureCouplingType>(
              ui->pressureCouplingType, simulation, "pressureCouplingType"
              );

  // temperature
  connect(ui->addTemperatureCouplingGroup, &QToolButton::clicked, [this] () {
    simulation->addTemperatureCouplingGroup();
  });
  connectToComboBox<Model::Simulation::TemperatureAlgorithm>(
              ui->temperatureAlgorithm, simulation, "temperatureAlgorithm"
              );

  setOptions(ui->electrostaticAlgorithm, {
     {"None", "no"},
     {"PME", "PME"},
  }, 1);
  connectToComboBox<QString>(ui->electrostaticAlgorithm, simulation, "electrostaticAlgorithm");
  connectToSpinBox<QDoubleSpinBox, double>(ui->electrostaticCutoffRadius, simulation, "electrostaticCutoffRadius");
  connectToSpinBox<QDoubleSpinBox, double>(ui->fourierSpacing, simulation, "fourierSpacing");
  connectToSpinBox<QDoubleSpinBox, double>(ui->vdwCutoffRadius, simulation, "vdwCutoffRadius");

  connect(simulation.get(), &Model::Simulation::temperatureCouplingGroupAdded,
          this, &SimulationSetupForm::addTemperatureCouplingGroup);
  connect(simulation.get(), &Model::Simulation::temperatureCouplingGroupRemoved,
          this, &SimulationSetupForm::removeTemperatureCouplingGroup);
  for (auto group : simulation->getTemperatureCouplingGroups())
  {
    addTemperatureCouplingGroup(group);
  }

  connect(ui->showLog, &QPushButton::clicked, [this] () {
    SimulationStatusChecker checker(project, simulation);
    auto viewer = new FileContentViewer(checker.getLogPath());
    viewer->show();
  });

  connect(ui->showMdp, &QPushButton::clicked, [this] () {
    SimulationStatusChecker checker(project, simulation);
    auto viewer = new FileContentViewer(checker.getMdpPath());
    viewer->show();
  });

  connect(ui->showTrajectoryButton, &QToolButton::clicked, [this] () {
    SimulationStatusChecker checker(project, simulation);
    if (checker.hasData())
    {
      emit displaySimulationData(checker.getCoordinatesPath(),
                                 checker.hasTrajectory() ? checker.getTrajectoryPath() : "");
    }
  });

  auto updateTimeStep = [this] (Simulation::Algorithm algorithm) {
    bool timeStepSupported = algorithm == Simulation::Algorithm::LeapFrog ||
      algorithm == Simulation::Algorithm::StochasticDynamics;
    ui->timeStep->setEnabled(timeStepSupported);
  };
  conns.push_back(connect(simulation.get(), &Simulation::algorithmChanged, updateTimeStep));
  updateTimeStep(simulation->property("algorithm").value<Simulation::Algorithm>());

  auto updateDuration = [this] () {
    const double numberOfSteps = simulation->property("numberOfSteps").value<double>();
    if (numberOfSteps < 1)
    {
      return;
    }

    const double timeStep = simulation->property("timeStep").value<double>();

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
  conns << connect(simulation.get(), &Simulation::numberOfStepsChanged, updateDuration);
  conns << connect(simulation.get(), &Simulation::timeStepChanged, updateDuration);
  updateDuration();

  conns << connect(
    ui->rerunSimulation,
    &QPushButton::clicked,
    [this] () {
      if (command->isRunning())
      {
        command->stop();
      }
      else
      {
        command->exec();
      }
    });

  conns << connect(
    command.get(),
    &Command::RunSimulation::progress,
    [this] (float progress, Command::Executor::ProgressType type) {
      if (type == Command::Executor::ProgressType::Value)
      {

        if (progress == 0)
        {
          return;
        }
        using namespace QtCharts;
        auto vertical = progressValueChart->axes(Qt::Vertical).first();
        auto horizontal = progressValueChart->axes(Qt::Horizontal).first();
        min = std::min<float>(min, progress);
        max = std::max<float>(max, progress);
        vertical->setRange(min, max);

        auto series = dynamic_cast<QSplineSeries*>(progressValueChart->series()[0]); 
        horizontal->setRange(0, series->count());

        auto point = QPointF(series->count(), progress);
        (*series) << point;
      }
      else
      {
        ui->simulationProgress->setValue(progress);
        if (firstProgressValue == -1)
        {
          firstProgressValue = progress;
        }

        QString assumedEndText("∞");
        // if simulation is resumed the percentage refers to the remaining time
        float actualProgress = (progress - firstProgressValue) / (100.0 - firstProgressValue) * 100.0;
        if (actualProgress > 0)
        {
          const qint64 currentTimeStamp = QDateTime::currentSecsSinceEpoch();
          const qint64 timePassed = currentTimeStamp - timeStampStarted;
          const qint64 assumedEndTimeStamp = timeStampStarted + (timePassed / actualProgress * 100);
          assumedEndText = QDateTime::fromSecsSinceEpoch(assumedEndTimeStamp).toString();
        }
        ui->assumedFinished->setText(assumedEndText);

      }
    });

  conns << connectToCheckbox(ui->resume, simulation, "resume");

  conns << connect(command.get(), &Command::RunSimulation::started,
                   [this] () {
                     timeStampStarted = QDateTime::currentSecsSinceEpoch();
                     series->removePoints(0, series->count());
                     firstProgressValue = -1;
                     min = INFINITY;
                     max = -INFINITY;
                     ui->rerunSimulation->setIcon(QIcon::fromTheme("media-playback-stop"));
                     ui->rerunSimulation->setText(tr("Stop Simulation"));
                     ui->simulationProgress->setEnabled(true);
                   });
  conns << connect(command.get(), &Command::RunSimulation::finished,
                   [this] () {
                     ui->rerunSimulation->setIcon(QIcon::fromTheme("reload"));
                     ui->rerunSimulation->setText(tr("Run simulation"));
                     ui->simulationProgress->setEnabled(false);
                     showEvent(nullptr);
                   });
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
  setProgressViewForType(type);
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

void SimulationSetupForm::setPressureAlgorithmsForType(Model::Simulation::Type type)
{
  using Model::Simulation;
  QList<QPair<QString, Simulation::PressureAlgorithm>> map({
    { "None", Simulation::PressureAlgorithm::None }
  });
  Simulation::PressureAlgorithm defaultValue = Simulation::PressureAlgorithm::None;
  if (type == Model::Simulation::Type::NPT)
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

void SimulationSetupForm::setProgressViewForType(Model::Simulation::Type type)
{
  const bool showChart = type == Model::Simulation::Type::Minimisation;
  chartView->setVisible(showChart);
  ui->simulationProgress->setVisible(!showChart);
  ui->assumedFinished->setVisible(!showChart);
}

void SimulationSetupForm::setTemperatureAlgorithmsForType(Model::Simulation::Type type)
{
  using Model::Simulation;
  QList<QPair<QString, Simulation::TemperatureAlgorithm>> map({
    { "None", Simulation::TemperatureAlgorithm::None }
  });
  Simulation::TemperatureAlgorithm defaultValue = Simulation::TemperatureAlgorithm::None;
  if (type == Model::Simulation::Type::NPT || type == Model::Simulation::Type::NVT)
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
    simulation->removeTemperatureCouplingGroup(at);
  });
  ui->temperatureCouplingGroups->insertWidget(at, groupEditor);
}

void SimulationSetupForm::removeTemperatureCouplingGroup(
  std::shared_ptr<Model::TemperatureCouplingGroup> group, int at)
{
  ui->temperatureCouplingGroups->takeAt(at)->widget()->deleteLater();
  if (!ui->temperatureCouplingGroups->count() ||
      group->property("group").value<Model::TemperatureCouplingGroup::Group>() ==
        Model::TemperatureCouplingGroup::Group::System)

  {
    ui->addTemperatureCouplingGroup->setEnabled(true);
  }
}

void SimulationSetupForm::showEvent(QShowEvent*)
{
  SimulationStatusChecker checker(project, simulation);
  ui->showTrajectoryButton->setEnabled(checker.hasData());
  ui->showLog->setEnabled(checker.hasLog());
  ui->showMdp->setEnabled(checker.hasMdp());
}

void SimulationSetupForm::setupProgressValueChart()
{
  using namespace QtCharts;

  progressValueChart = new QtCharts::QChart();

  progressValueChart->setMargins(QMargins());
  progressValueChart->legend()->hide();
  progressValueChart->layout()->setContentsMargins(0, 6, 0, 0);
  progressValueChart->setBackgroundVisible(false);

  series = new QSplineSeries();
  progressValueChart->addSeries(series);

  progressValueChart->createDefaultAxes();
  auto horizontal = progressValueChart->axes(Qt::Horizontal).first();
  horizontal->setVisible(false);
  auto vertical = progressValueChart->axes(Qt::Vertical).first();
  vertical->setVisible(false);

  chartView = new QtCharts::QChartView(progressValueChart);
  chartView->setFrameStyle(QFrame::NoFrame);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->setStyleSheet("background-color: transparent;");
  chartView->setMinimumWidth(180);
  chartView->setVisible(false);
  ui->runLayout->addWidget(chartView);
}
