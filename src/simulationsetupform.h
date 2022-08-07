#ifndef SIMULATIONSETUPFORM_H
#define SIMULATIONSETUPFORM_H

#include "model/simulationtype.h"
#include "model/step.h"

#include <QWidget>
#include <QComboBox>
#include <memory>

namespace Ui {
class SimulationSetupForm;
}

class SimulationSetupForm : public QWidget
{
    Q_OBJECT

public:
    explicit SimulationSetupForm(std::shared_ptr<Step> newStep, QWidget *parent = nullptr);
    ~SimulationSetupForm();

private:
    std::shared_ptr<Step> step;
    Ui::SimulationSetupForm *ui;

    void updateUiForSimulationType(SimulationType type = SimulationType::None);
    void setAlgorithmsForType(SimulationType type);
    void hideSettings();
    void enableAllSettings();
};

#endif // SIMULATIONSETUPFORM_H
