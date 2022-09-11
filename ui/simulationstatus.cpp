#include "simulationstatus.h"
#include "../src/pipeline/simulation/configuration.h"
#include "../src/pipeline/simulation/command.h"
#include "ui_simulationstatus.h"
#include "../src/simulationstatuschecker.h"
#include "../src/filecontentviewer.h"
#include "../src/projectmanager.h"
#include "../src/appprovider.h"
#include "../src/form/connectionhelper.h"
#include <memory>
#include <QToolButton>
#include <QDateTime>
#include <QProcess>
#include <variant>

SimulationStatus::SimulationStatus(
  Pipeline::Simulation::Command* newStep,
  Pipeline::Simulation::Configuration* newConfiguration,
  QWidget *parent
  )
    : QWidget(parent)
    , step(newStep)
    , configuration(newConfiguration)
    , ui(new Ui::SimulationStatus)
{
  ui->setupUi(this);
  setupProgressValueChart();

  auto type = newConfiguration
    ->property("simulationType")
    .value<Pipeline::Simulation::Configuration::Type>();
  setProgressViewForType(type);

  conns << connect(ui->showLog, &QPushButton::clicked, [this] () {
    auto project = ProjectManager::getInstance()->getCurrentProject();
    SimulationStatusChecker checker(project, configuration);
    auto viewer = new FileContentViewer(checker.getLogPath());
    viewer->show();
  });

  conns << connect(ui->showMdp, &QPushButton::clicked, [this] () {
    auto project = ProjectManager::getInstance()->getCurrentProject();
    SimulationStatusChecker checker(project, configuration);
    auto viewer = new FileContentViewer(checker.getMdpPath());
    viewer->show();
  });

  conns << connect(ui->showSmoothTrajectory, &QToolButton::clicked, [this] () {
    auto project = ProjectManager::getInstance()->getCurrentProject();
    SimulationStatusChecker checker(project, configuration);
    if (checker.hasTrajectory())
    {
      QProcess createInput;
      QString command = AppProvider::get("gmx");
      command += " filter";
      command += " -s " + checker.getTprPath();
      command += " -f " + checker.getTrajectoryPath();
      command += " -ol " + checker.getSmoothTrajectoryPath();
      command += " -all -nojump -nf 5";
      createInput.start(command);
      createInput.waitForFinished();
      //emit displaySimulationData(checker.getInputCoordinatesPath(),
      //                           checker.getSmoothTrajectoryPath());
    }
  });
  conns << connect(ui->showTrajectoryButton, &QToolButton::clicked, [this] () {
    auto project = ProjectManager::getInstance()->getCurrentProject();
    SimulationStatusChecker checker(project, configuration);
    if (checker.hasData())
    {
      //emit displaySimulationData(checker.getInputCoordinatesPath(),
      //                           checker.hasTrajectory() ? checker.getTrajectoryPath() : "");
    }
  });

  conns << connect(
    ui->rerunSimulation,
    &QPushButton::clicked,
    [this] () {
      if (step->isRunning())
      {
        step->stop();
      }
      else
      {
        step->exec();
      }
    });

  conns << connect(
    step,
    &Pipeline::Simulation::Command::progress,
    [this] (float progress, Command::Executor::ProgressType type) {
      if (type == Command::Executor::ProgressType::Value)
      {

        if (progress == 0)
        {
          return;
        }
        progressChart->appendValue(progress);
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

  conns << connectToCheckbox(ui->resume, configuration, "resume");

  conns << connect(
    step, &Pipeline::Simulation::Command::started,
    [this] () {
      progressChart->clear();

      timeStampStarted = QDateTime::currentSecsSinceEpoch();
      firstProgressValue = -1;
      ui->rerunSimulation->setIcon(QIcon::fromTheme("media-playback-stop"));
      ui->rerunSimulation->setText(tr("Stop Simulation"));
      ui->simulationProgress->setEnabled(true);
    });
  conns << connect(
    step, &Pipeline::Simulation::Command::finished,
    [this] () {
      ui->rerunSimulation->setIcon(QIcon::fromTheme("reload"));
      ui->rerunSimulation->setText(tr("Run simulation"));
      ui->simulationProgress->setEnabled(false);
      showEvent(nullptr);
    });

  auto project = ProjectManager::getInstance()->getCurrentProject();
  SimulationStatusChecker checker(project, configuration);
  if (checker.hasLog())
  {
    if (configuration->isMinimisation())
    {
      progressChart->setValues(checker.getProgressValues());
    }
    else
    {
      ui->simulationProgress->setValue(checker.getProgress());
    }
  }
}

void SimulationStatus::setProgressViewForType(Pipeline::Simulation::Configuration::Type type)
{
  const bool showChart = type == Pipeline::Simulation::Configuration::Type::Minimisation;
  progressChart->setVisible(showChart);
  ui->simulationProgress->setVisible(!showChart);
  ui->assumedFinished->setVisible(!showChart);
}

void SimulationStatus::setupProgressValueChart()
{
  using namespace QtCharts;

  progressChart = new Gui::ProgressChart;

  progressChart->setVisible(false);
  progressChart->setMinimumWidth(180);
  progressChart->setMaximumWidth(180);
  progressChart->setMaximumHeight(40);
  ui->runLayout->addWidget(progressChart);
}

void SimulationStatus::showEvent(QShowEvent*)
{

  auto project = ProjectManager::getInstance()->getCurrentProject();

  SimulationStatusChecker checker(project, configuration);
  ui->showTrajectoryButton->setEnabled(checker.hasData());
  ui->showSmoothTrajectory->setEnabled(checker.hasTrajectory());
  ui->showLog->setEnabled(checker.hasLog());
  ui->showMdp->setEnabled(checker.hasMdp());
}
SimulationStatus::~SimulationStatus()
{
    delete ui;
}
