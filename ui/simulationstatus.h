#ifndef SIMULATIONSTATUS_H
#define SIMULATIONSTATUS_H

#include <QWidget>
#include <QList>
#include "src/form/progresschart.h"
#include "../src/pipeline/simulation/configuration.h"

namespace Ui {
class SimulationStatus;
}

namespace Pipeline { namespace Simulation {
class Command;
} }

class SimulationStatus : public QWidget
{
  Q_OBJECT

public:
  explicit SimulationStatus(
    Pipeline::Simulation::Command* step,
    Pipeline::Simulation::Configuration* configuration,
    QWidget *parent = nullptr);
  ~SimulationStatus();

private:
  void setProgressViewForType(Pipeline::Simulation::Configuration::Type type);
  void setupProgressValueChart();
  Pipeline::Simulation::Command* step;
  Pipeline::Simulation::Configuration* configuration;
  Ui::SimulationStatus *ui;
  QList<QMetaObject::Connection> conns;
  Gui::ProgressChart* progressChart;

  qint64 timeStampStarted;
  float firstProgressValue = -1;
};

#endif // SIMULATIONSTATUS_H
