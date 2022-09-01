#ifndef SIMULATIONSTATUS_H
#define SIMULATIONSTATUS_H

#include <QWidget>
#include <QList>
#include "../src/config/simulation.h"
#include "src/model/project.h"
#include "src/command/runsimulation.h"
#include "src/form/progresschart.h"

namespace Ui {
class SimulationStatus;
}

class SimulationStatus : public QWidget
{
  Q_OBJECT

public:
  explicit SimulationStatus(
    std::shared_ptr<Command::RunSimulation> step,
    std::shared_ptr<Config::Simulation> configuration,
    QWidget *parent = nullptr);
  ~SimulationStatus();
  void showEvent(QShowEvent* event);

private:
  void setProgressViewForType(Config::Simulation::Type type);
  void setupProgressValueChart();
  std::shared_ptr<Command::RunSimulation> step;
  std::shared_ptr<Config::Simulation> configuration;
  Ui::SimulationStatus *ui;
  QList<QMetaObject::Connection> conns;
  Gui::ProgressChart* progressChart;

  qint64 timeStampStarted;
  float firstProgressValue = -1;
};

#endif // SIMULATIONSTATUS_H
