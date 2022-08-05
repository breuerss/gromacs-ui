#ifndef SIMULATIONSETUPFORM_H
#define SIMULATIONSETUPFORM_H

#include "simulationtype.h"
#include <QWidget>
#include <QComboBox>
#include <memory>
#include "step.h"

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

    void updateUiForSimulationType(SimulationType::Type type = SimulationType::None);
    void setAlgorithmsForType(SimulationType::Type type);
    void hideSettings();
    void enableAllSettings();
};

#endif // SIMULATIONSETUPFORM_H
