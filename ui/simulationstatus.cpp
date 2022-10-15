#include "simulationstatus.h"
#include "ui_simulationstatus.h"
#include "connectionhelper.h"

#include "../src/pipeline/simulation/configuration.h"
#include "../src/pipeline/simulation/command.h"
#include "../src/simulationstatuschecker.h"
#include "../src/projectmanager.h"
#include "../src/appprovider.h"
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

  auto runningCallback = [this] () {
    progressChart->clear();

    timeStampStarted = QDateTime::currentSecsSinceEpoch();
    firstProgressValue = -1;
    ui->simulationProgress->setEnabled(true);
  };
  conns << connect(
    step, &Pipeline::Simulation::Command::started, runningCallback
    );
  auto stoppedCallback = [this] () {
    ui->simulationProgress->setEnabled(false);
    showEvent(nullptr);
  };
  conns << connect(
    step, &Pipeline::Simulation::Command::finished, stoppedCallback
    );

  if (step->isRunning())
  {
    runningCallback();
  }

  auto project = ProjectManager::getInstance()->getCurrentProject();
  SimulationStatusChecker checker(project, configuration);
  if (configuration->isMinimisation())
  {
    progressChart->setValues(checker.getProgressValues());
  }
  else
  {
    ui->simulationProgress->setValue(checker.getProgress());
  }
}

SimulationStatus::~SimulationStatus()
{
  for (auto conn: conns)
  {
    disconnect(conn);
  }

  delete ui;
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
