#include "systemsetupform.h"
#include "ui_systemsetupform.h"
#include "pdbdownloader.h"
#include "pdbinfoextractor.h"
#include "model/project.h"
#include <QDir>
#include <QCheckBox>
#include <QTimer>
#include "uiconnectionhelper.h"
#include "command/queue.h"
#include "command/creategromacsmodel.h"
#include "command/createbox.h"
#include "command/solvate.h"
#include "command/neutralise.h"

SystemSetupForm::SystemSetupForm(std::shared_ptr<SystemSetup> newSystemSetup, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SystemSetupForm)
    , systemSetup(newSystemSetup)
{
    ui->setupUi(this);

    prepareWaterOptions();
    prepareForceFieldOptions();
    prepareBoxOptions();
    setIonFromModel();
    connectIonSelectors();


    auto queue = std::make_shared<Command::Queue>();
    connect(queue.get(), &Command::Queue::finished, [this] (bool success) {
        systemSetup->setStructureReady(success);
    });
    connect(systemSetup.get(), &SystemSetup::configReadyChanged, [this, queue] (bool ready) {
        if (ready)
        {
            systemSetup->setStructureReady(false);
            queue
                ->clear()
                ->enqueue(std::make_shared<Command::CreateGromacsModel>(systemSetup))
                ->enqueue(std::make_shared<Command::CreateBox>(systemSetup))
                ->enqueue(std::make_shared<Command::Solvate>(systemSetup))
                ->enqueue(std::make_shared<Command::Neutralise>(systemSetup))
                ->start();
        }
    });

    connectToComboBox<QString>(ui->boxType, systemSetup, "boxType");
    connectToComboBox<QString>(ui->waterModel, systemSetup, "waterModel");
    connectToComboBox<QString>(ui->forceField, systemSetup, "forceField");

    connectToSpinBox<QDoubleSpinBox, double>(ui->distanceToEdge, systemSetup, "distance");
    connectToSpinBox<QDoubleSpinBox, double>(ui->ionConcentration, systemSetup, "ionConcentration");

    connectToCheckbox(ui->removeHeteroAtoms, systemSetup, "removeHeteroAtoms");

    connectToLineEdit(ui->pdbEntry, systemSetup, "pdbCode", [this] (const QString& pdbCode) {
        if (pdbCode.length() == 4)
        {
            qDebug() << "Starting PDB download for" << pdbCode;
            auto* pdbDownloader = new PdbDownloader();
            connect(pdbDownloader, &PdbDownloader::downloaded,
                    [pdbDownloader, this] (const QString& /*pdbCode*/, const QString& file) {
                qDebug() << "downloaded" << file;
                systemSetup->setSourceStructureFile(file);
                pdbDownloader->deleteLater();
            });

            // TODO does that belong here? Isn't that part of the project setup?
            QDir dir(systemSetup->getProject()->getProjectPath());
            if (dir.mkpath("input"))
            {
                dir.cd("input");
            }
            QString absFilePath = dir.absolutePath() + "/" + pdbCode + ".pdb";
            pdbDownloader->download(pdbCode, absFilePath);
        }
        else
        {
            setGroupsEnabled(false);
        }
    });

    connect(systemSetup.get(), &SystemSetup::sourceStructureFileChanged,
        [this] (const QString& sourceStructureFile) {
        // Workaround since this callback is executed before the
        // callback that should update the molecule preview.
        QTimer::singleShot(100, [this, sourceStructureFile] {

            if (!sourceStructureFile.isEmpty())
            {
                setGroupsEnabled(true);
                PdbInfoExtractor extractor;
                QStringList chains = extractor.getChains(sourceStructureFile);

                QLayoutItem* item;
                while ((item = ui->chainsGroupLayout->takeAt(0)) != 0)
                {
                    if (item->widget())
                    {
                        delete item->widget();
                    }
                    delete item;
                }

                systemSetup->setChains(chains);
                for (const auto& chain: chains)
                {
                    QCheckBox* checkBox = new QCheckBox(chain);
                    checkBox->setCheckState(Qt::Checked);
                    connect(checkBox, &QCheckBox::stateChanged, [this, chain] (int state) {
                         systemSetup->useChain(chain, state == Qt::Checked);
                    });
                    ui->chainsGroupLayout->addWidget(checkBox);
                }
            }
            });
    });
}

SystemSetupForm::~SystemSetupForm()
{
    disconnect(systemSetup.get(), 0, 0, 0);
    delete ui;
}

void SystemSetupForm::prepareWaterOptions()
{
    setOptions(ui->waterModel, {
        { "SPC", "spc" },
        { "TIP3P", "tip3p" },
        { "TIP4P", "tip4p" }
    }, 1);
}

void SystemSetupForm::prepareForceFieldOptions()
{
    setOptions(ui->forceField, {
        { "CHARMM27", "charmm27" },
        { "OPLS-AA/L", "oplsaa" }
    }, 0);
}

void SystemSetupForm::prepareBoxOptions()
{
    setOptions(ui->boxType, {
        { "Cubic", "cubic" },
        { "Octahedron", "octahedron" },
        { "Dodecahedron", "dodecahedron" }
    }, 2);
}

void SystemSetupForm::setIonFromModel()
{
    QString positiveIon = systemSetup->property("positiveIon").value<QString>();
    QString negativeIon = systemSetup->property("negativeIon").value<QString>();
    QMap<QString, QRadioButton*> map = {
        { "MG", ui->mgIon },
        { "CA", ui->caIon },
        { "LI", ui->liIon },
        { "NA", ui->naIon },
        { "K", ui->kIon },
        { "RB", ui->rbIon },
        { "CS", ui->csIon },
        { "F", ui->fIon },
        { "CL", ui->clIon },
        { "BR", ui->brIon },
        { "I", ui->iIon },
    };

    for (const auto& element : map.keys())
    {
        if (positiveIon == element || negativeIon == element)
        {
            map[element]->setChecked(true);
        }
    }

}

void SystemSetupForm::connectIonSelectors()
{
    QMap<QRadioButton*, QString> positiveIonMap = {
        { ui->mgIon, "MG" },
        { ui->caIon, "CA" },
        { ui->liIon, "LI" },
        { ui->naIon, "NA" },
        { ui->kIon, "K" },
        { ui->rbIon, "RB" },
        { ui->csIon, "CS" },
    };
    for (auto* button: positiveIonMap.keys())
    {
        QString ion = positiveIonMap[button];
        connect(button, &QRadioButton::toggled,
            [this, ion] (bool checked)
        {
            if (checked)
            {
                systemSetup->setProperty("positiveIon", ion);
            }
        });
    }

    QMap<QRadioButton*, QString> negativeIonMap = {
        { ui->fIon, "F" },
        { ui->clIon, "CL" },
        { ui->brIon, "BR" },
        { ui->iIon, "I" },
    };
    for (auto* button: negativeIonMap.keys())
    {
        QString ion = negativeIonMap[button];
        connect(button, &QRadioButton::toggled,
            [this, ion] (bool checked)
        {
            if (checked)
            {
                systemSetup->setProperty("negativeIon", ion);
            }
        });
    }
}

void SystemSetupForm::setGroupsEnabled(bool enabled)
{
    ui->filterGroup->setEnabled(enabled);
    ui->systemSetupGroup->setEnabled(enabled);
    ui->neutraliseGroup->setEnabled(enabled);
}
