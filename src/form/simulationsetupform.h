#ifndef SIMULATIONSETUPFORM_H
#define SIMULATIONSETUPFORM_H

#include <QWidget>
#include <QComboBox>
#include <memory>
#include <QMetaObject>
#include <vector>
#include "../model/simulation.h"

namespace Model {
class Project;
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
    QWidget *parent = nullptr);
  ~SimulationSetupForm();

  void showEvent(QShowEvent* event);

signals:
  void displaySimulationData(const QString& coords, const QString& trajectory);

private:
  std::shared_ptr<Model::Project> project;
  std::shared_ptr<Model::Simulation> simulation;
  Ui::SimulationSetupForm *ui;

  void updateUiForSimulationType(Model::Simulation::Type type = Model::Simulation::Type::None);
  void setAlgorithmsForType(Model::Simulation::Type type);
  void setPressureAlgorithmsForType(Model::Simulation::Type type);
  void setTemperatureAlgorithmsForType(Model::Simulation::Type type);
  void hideSettings();
  void enableAllSettings();

  void addTemperatureCouplingGroup(std::shared_ptr<Model::TemperatureCouplingGroup> couplingGroup, int at = -1);
  void removeTemperatureCouplingGroup(std::shared_ptr<Model::TemperatureCouplingGroup> couplingGroup, int at);
  std::vector<QMetaObject::Connection> conns;
};

#endif // SIMULATIONSETUPFORM_H
