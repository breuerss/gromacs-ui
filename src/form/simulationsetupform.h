#ifndef SIMULATIONSETUPFORM_H
#define SIMULATIONSETUPFORM_H

#include <QWidget>
#include <QComboBox>
#include <memory>
#include <QMetaObject>
#include <QList>
#include "../config/simulation.h"
#include "qglobal.h"

namespace Ui {
class SimulationSetupForm;
}

class SimulationSetupForm : public QWidget
{
  Q_OBJECT

public:
  ~SimulationSetupForm();

  SimulationSetupForm(std::shared_ptr<Config::Simulation> config);

signals:
  void displaySimulationData(const QString& coords, const QString& trajectory);

private:
  std::shared_ptr<Config::Simulation> simulation;
  Ui::SimulationSetupForm *ui;

  void updateUiForSimulationType(Config::Simulation::Type type = Config::Simulation::Type::None);
  void setAlgorithmsForType(Config::Simulation::Type type);
  void setPressureAlgorithmsForType(Config::Simulation::Type type);
  void setTemperatureAlgorithmsForType(Config::Simulation::Type type);
  void hideSettings();
  void enableAllSettings();

  void addTemperatureCouplingGroup(std::shared_ptr<Config::TemperatureCouplingGroup> couplingGroup, int at = -1);
  void removeTemperatureCouplingGroup(std::shared_ptr<Config::TemperatureCouplingGroup> couplingGroup, int at);
  QList<QMetaObject::Connection> conns;
};

#endif // SIMULATIONSETUPFORM_H
