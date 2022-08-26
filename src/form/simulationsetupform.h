#ifndef SIMULATIONSETUPFORM_H
#define SIMULATIONSETUPFORM_H

#include <QWidget>
#include <QComboBox>
#include <memory>
#include <QMetaObject>
#include <QList>
#include "../config/simulation.h"
#include "qglobal.h"
#include "progresschart.h"

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
    std::shared_ptr<Config::Simulation> simulation,
    std::shared_ptr<Command::RunSimulation> command,
    QWidget *parent = nullptr);
  ~SimulationSetupForm();

  void showEvent(QShowEvent* event);

signals:
  void displaySimulationData(const QString& coords, const QString& trajectory);

private:
  std::shared_ptr<Model::Project> project;
  std::shared_ptr<Config::Simulation> simulation;
  std::shared_ptr<Command::RunSimulation> command;
  Ui::SimulationSetupForm *ui;

  void updateUiForSimulationType(Config::Simulation::Type type = Config::Simulation::Type::None);
  void setAlgorithmsForType(Config::Simulation::Type type);
  void setProgressViewForType(Config::Simulation::Type type);
  void setPressureAlgorithmsForType(Config::Simulation::Type type);
  void setTemperatureAlgorithmsForType(Config::Simulation::Type type);
  void hideSettings();
  void enableAllSettings();

  void addTemperatureCouplingGroup(std::shared_ptr<Config::TemperatureCouplingGroup> couplingGroup, int at = -1);
  void removeTemperatureCouplingGroup(std::shared_ptr<Config::TemperatureCouplingGroup> couplingGroup, int at);
  QList<QMetaObject::Connection> conns;

  qint64 timeStampStarted;
  float firstProgressValue = -1;

  void setupProgressValueChart();
  Gui::ProgressChart* progressChart;
};

#endif // SIMULATIONSETUPFORM_H
