#ifndef SIMULATIONSETUPFORM_H
#define SIMULATIONSETUPFORM_H

#include <QWidget>
#include <QComboBox>
#include <memory>
#include <QMetaObject>
#include <QList>
#include "../pipeline/simulation/configuration.h"
#include "../pipeline/simulation/temperaturecouplinggroup.h"

namespace Ui {
class SimulationSetupForm;
}

class SimulationSetupForm : public QWidget
{
  Q_OBJECT

public:
  using Configuration = Pipeline::Simulation::Configuration; 
  SimulationSetupForm(Configuration* config);
  ~SimulationSetupForm();

signals:
  void displaySimulationData(const QString& coords, const QString& trajectory);

private:
  Ui::SimulationSetupForm *ui;
  using SimConfiguration = Pipeline::Simulation::Configuration;
  SimConfiguration* simulation;

  void updateUiForSimulationType(SimConfiguration::Type type = SimConfiguration::Type::None);
  void setAlgorithmsForType(SimConfiguration::Type type);
  void setPressureAlgorithmsForType(SimConfiguration::Type type);
  void setTemperatureAlgorithmsForType(SimConfiguration::Type type);
  void hideSettings();
  void enableAllSettings();

  void addTemperatureCouplingGroup(std::shared_ptr<Pipeline::Simulation::TemperatureCouplingGroup> couplingGroup, int at = -1);
  void removeTemperatureCouplingGroup(std::shared_ptr<Pipeline::Simulation::TemperatureCouplingGroup> couplingGroup, int at);
  QList<QMetaObject::Connection> conns;
};

#endif // SIMULATIONSETUPFORM_H
