#ifndef SIMULATIONSETUPFORM_H
#define SIMULATIONSETUPFORM_H

#include <QWidget>
#include <QComboBox>
#include <memory>
#include "model/simulation.h"

namespace Ui {
class SimulationSetupForm;
}

class SimulationSetupForm : public QWidget
{
  Q_OBJECT

public:
  explicit SimulationSetupForm(std::shared_ptr<Model::Simulation> newStep, QWidget *parent = nullptr);
  ~SimulationSetupForm();

private:
  std::shared_ptr<Model::Simulation> step;
  Ui::SimulationSetupForm *ui;

  void updateUiForSimulationType(Model::Simulation::Type type = Model::Simulation::Type::None);
  void setAlgorithmsForType(Model::Simulation::Type type);
  void setPressureAlgorithmsForType(Model::Simulation::Type type);
  void hideSettings();
  void enableAllSettings();
};

#endif // SIMULATIONSETUPFORM_H
