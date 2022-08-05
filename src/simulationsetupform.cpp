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
        { "None", SimulationType::None },
        { "Minimisation", SimulationType::Minimisation },
//        { "Microcanonical (NVE)", SimulationType::NVE },
//        { "Canonical (NVT)", SimulationType::NVT },
//        { "Gibbs/Isobaric-isothermal (NPT)", SimulationType::NPT },
    });

    connectToUi<SimulationType::Type>(
                ui->simulationType,
                step,
                "simulationType",
                [this] (SimulationType::Type type) {
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

    setOptions(ui->electroStaticAlgorithm, {
                   {"PME", "PME"},
               });
    connectToUi<QString>(container, step, "electroStaticAlgorithm");
    connectToUi<QDoubleSpinBox, double>(container, step, "electrostaticCutoffRadius");
    connectToUi<QDoubleSpinBox, double>(container, step, "fourierSpacing");
    connectToUi<QDoubleSpinBox, double>(container, step, "vdwCutoffRadius");

    connect(ui->createConfig, &QPushButton::clicked, [this] () {
        GromacsConfigFileGenerator::generate(step, "/tmp/out.mdp");
    });

}

SimulationSetupForm::~SimulationSetupForm()
{
    delete ui;
}

void SimulationSetupForm::updateUiForSimulationType(SimulationType::Type type)
{
    hideSettings();
    setAlgorithmsForType(type);
    enableAllSettings();
    switch(type)
    {
    case SimulationType::Minimisation:
        ui->electroStaticsGroup->setVisible(true);
        ui->vanDerWaalsGroup->setVisible(true);
        ui->generalGroup->setVisible(true);
        ui->outputSettingsGroup->setVisible(true);
        ui->minimisationGroup->setVisible(true);
        ui->positionOutputFrequency->setEnabled(false);
        ui->velocityOutputFrequency->setEnabled(false);
        ui->forceOutputFrequency->setEnabled(false);
        break;
    case SimulationType::None:
    default:
        break;
    }
}

void SimulationSetupForm::hideSettings()
{
    ui->electroStaticsGroup->setVisible(false);
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

void SimulationSetupForm::setAlgorithmsForType(SimulationType::Type type)
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
    default:
        break;
    }

    for (const auto& entry: map)
    {
        ui->algorithm->addItem(entry.first, entry.second);
    }
    ui->algorithm->setCurrentIndex(defaultIndex);

}
