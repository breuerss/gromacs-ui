#include "simulationsetupform.h"
#include "ui_simulationsetupform.h"
#include "model/simulationtype.h"
#include "gromacsconfigfilegenerator.h"
#include "uiconnectionhelper.h"

SimulationSetupForm::SimulationSetupForm(std::shared_ptr<Step> newStep, QWidget *parent) :
    QWidget(parent)
  , step(newStep)
  , ui(new Ui::SimulationSetupForm)
{
    ui->setupUi(this);
    setOptions(ui->simulationType, {
        { "None", QVariant::fromValue(SimulationType::None) },
        { "Minimisation", QVariant::fromValue(SimulationType::Minimisation) },
//        { "Microcanonical (NVE)", SimulationType::NVE },
        { "Canonical (NVT)", QVariant::fromValue(SimulationType::NVT) },
//        { "Gibbs/Isobaric-isothermal (NPT)", SimulationType::NPT },
    });

    connectToUi<SimulationType>(
                ui->simulationType,
                step,
                "simulationType",
                [this] (SimulationType type) {
        updateUiForSimulationType(type);
    });

    QWidget* container = ui->settingsWidget;
    connectToUi<QString>(container, step, "algorithm");

    connectToUi<QSpinBox, int>(container, step, "numberOfSteps");
    connectToUi<QDoubleSpinBox, double>(container, step, "minimisationStepSize");
    connectToUi<QDoubleSpinBox, double>(container, step, "minimisationMaximumForce");
    connectToUi<QSpinBox, int>(container, step, "energyOutputFrequency");
    connectToUi<QSpinBox, int>(container, step, "positionOutputFrequency");
    connectToUi<QSpinBox, int>(container, step, "velocityOutputFrequency");
    connectToUi<QSpinBox, int>(container, step, "forceOutputFrequency");

    setOptions(ui->electrostaticAlgorithm, {
                   {"None", "no"},
                   {"PME", "PME"},
               }, 1);
    connectToUi<QString>(ui->electrostaticAlgorithm, step, "electrostaticAlgorithm");
    connectToUi<QDoubleSpinBox, double>(ui->electrostaticCutoffRadius, step, "electrostaticCutoffRadius");
    connectToUi<QDoubleSpinBox, double>(ui->fourierSpacing, step, "fourierSpacing");
    connectToUi<QDoubleSpinBox, double>(ui->vdwCutoffRadius, step, "vdwCutoffRadius");
}

SimulationSetupForm::~SimulationSetupForm()
{
    delete ui;
}

void SimulationSetupForm::updateUiForSimulationType(SimulationType type)
{
    hideSettings();
    setAlgorithmsForType(type);
    enableAllSettings();
    switch(type)
    {
    case SimulationType::Minimisation:
        ui->electrostaticsGroup->setVisible(true);
        ui->vanDerWaalsGroup->setVisible(true);
        ui->generalGroup->setVisible(true);
        ui->outputSettingsGroup->setVisible(true);
        ui->minimisationGroup->setVisible(true);
        ui->positionOutputFrequency->setEnabled(false);
        ui->velocityOutputFrequency->setEnabled(false);
        ui->forceOutputFrequency->setEnabled(false);
        break;
    case SimulationType::NVT:
        ui->electrostaticsGroup->setVisible(true);
        ui->vanDerWaalsGroup->setVisible(true);
        ui->generalGroup->setVisible(true);
        ui->outputSettingsGroup->setVisible(true);
        break;
    case SimulationType::None:
    default:
        break;
    }
}

void SimulationSetupForm::hideSettings()
{
    ui->electrostaticsGroup->setVisible(false);
    ui->vanDerWaalsGroup->setVisible(false);
    ui->generalGroup->setVisible(false);
    ui->outputSettingsGroup->setVisible(false);
    ui->minimisationGroup->setVisible(false);
    ui->temperatureGroup->setVisible(false);
    ui->pressureGroup->setVisible(false);
}

void SimulationSetupForm::enableAllSettings()
{
    for (QWidget* child: ui->settingsWidget->findChildren<QWidget*>())
    {
        child->setEnabled(true);
    }
}

void SimulationSetupForm::setAlgorithmsForType(SimulationType type)
{
    QList<QPair<QString, QVariant>> map;
    int defaultIndex = 0;

    switch(type)
    {
    case SimulationType::Minimisation:
        map = QList<QPair<QString, QVariant>>({
            { "None", "" },
            { "Steepest Descent", "steep" },
            { "Conjugate Gradient", "cg" },
        });
        defaultIndex = 1;
        break;
    case SimulationType::NVT:
        map = QList<QPair<QString, QVariant>>({
            { "None", "" },
            { "Leap Frog", "md" },
        });
        defaultIndex = 1;
    default:
        break;
    }

    setOptions(ui->algorithm, map, defaultIndex);
}
