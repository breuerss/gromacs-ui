#ifndef SIMULATIONSETUPFORM_H
#define SIMULATIONSETUPFORM_H

#include <QWidget>
#include <QComboBox>
#include <cmath>
#include <memory>
#include <QMetaObject>
#include <QList>
#include <QChart>
#include <QSplineSeries>
#include <QChartView>
#include "../model/simulation.h"
#include "qglobal.h"

namespace Model {
class Project;
}
namespace Command {
class RunSimulation;
}
namespace Ui {
class SimulationSetupForm;
}

class SimulationSetupForm : public QWidget
{
  Q_OBJECT

public:
  explicit SimulationSetupForm(
    std::shared_ptr<Model::Project> project,
    std::shared_ptr<Model::Simulation> simulation,
    std::shared_ptr<Command::RunSimulation> command,
    QWidget *parent = nullptr);
  ~SimulationSetupForm();

  void showEvent(QShowEvent* event);

signals:
  void displaySimulationData(const QString& coords, const QString& trajectory);

private:
  std::shared_ptr<Model::Project> project;
  std::shared_ptr<Model::Simulation> simulation;
  std::shared_ptr<Command::RunSimulation> command;
  Ui::SimulationSetupForm *ui;

  void updateUiForSimulationType(Model::Simulation::Type type = Model::Simulation::Type::None);
  void setAlgorithmsForType(Model::Simulation::Type type);
  void setProgressViewForType(Model::Simulation::Type type);
  void setPressureAlgorithmsForType(Model::Simulation::Type type);
  void setTemperatureAlgorithmsForType(Model::Simulation::Type type);
  void hideSettings();
  void enableAllSettings();

  void addTemperatureCouplingGroup(std::shared_ptr<Model::TemperatureCouplingGroup> couplingGroup, int at = -1);
  void removeTemperatureCouplingGroup(std::shared_ptr<Model::TemperatureCouplingGroup> couplingGroup, int at);
  QList<QMetaObject::Connection> conns;

  qint64 timeStampStarted;
  float firstProgressValue = -1;

  QtCharts::QChart* progressValueChart;
  QtCharts::QChartView* chartView;
  QtCharts::QSplineSeries* series;
  void setupProgressValueChart();
  float min = INFINITY;
  float max = -INFINITY;
};

#endif // SIMULATIONSETUPFORM_H
