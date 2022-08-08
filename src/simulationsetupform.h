#ifndef SIMULATIONSETUPFORM_H
#define SIMULATIONSETUPFORM_H

#include "model/simulationtype.h"

#include <QWidget>
#include <QComboBox>
#include <memory>

namespace Model {
class Step;
}

namespace Ui {
class SimulationSetupForm;
}

class SimulationSetupForm : public QWidget
{
  Q_OBJECT

public:
  explicit SimulationSetupForm(std::shared_ptr<Model::Step> newStep, QWidget *parent = nullptr);
  ~SimulationSetupForm();

private:
  std::shared_ptr<Model::Step> step;
  Ui::SimulationSetupForm *ui;

  void updateUiForSimulationType(Model::SimulationType type = Model::SimulationType::None);
  void setAlgorithmsForType(Model::SimulationType type);
  void hideSettings();
  void enableAllSettings();
};

#endif // SIMULATIONSETUPFORM_H
