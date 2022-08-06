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

SystemSetupForm::SystemSetupForm(std::shared_ptr<SystemSetup> newSystemSetup, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SystemSetupForm)
    , systemSetup(newSystemSetup)
{
    ui->setupUi(this);

    prepareWaterOptions();
    prepareForceFieldOptions();
    prepareBoxOptions();

    auto queue = std::make_shared<Command::Queue>();
    connect(systemSetup.get(), &SystemSetup::configReadyChanged, [this, queue] (bool ready) {
        qDebug() << "config ready";
        if (ready)
        {
            queue
                ->clear()
                ->enqueue(std::make_shared<Command::CreateGromacsModel>(systemSetup))
                ->enqueue(std::make_shared<Command::CreateBox>(systemSetup))
                ->enqueue(std::make_shared<Command::Solvate>(systemSetup))
                ->start();
            // TODO genion
        }
    });

    connect(ui->boxType, QOverload<int>::of(&QComboBox::currentIndexChanged), [this] (int) {
       systemSetup->setBoxType(ui->boxType->currentData().toString());
    });
    // TODO set values of UI based on model and not the other way around
    systemSetup->setBoxType(ui->boxType->currentData().toString());

    connect(ui->waterModel, QOverload<int>::of(&QComboBox::currentIndexChanged), [this] (int) {
       systemSetup->setWaterModel(ui->waterModel->currentData().toString());
    });
    systemSetup->setWaterModel(ui->waterModel->currentData().toString());

    connect(ui->distanceToEdge, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this] (double distance) {
        systemSetup->setDistance(distance);
    });
    systemSetup->setDistance(ui->distanceToEdge->value());

    connect(ui->forceField, QOverload<int>::of(&QComboBox::currentIndexChanged), [this] (int) {
       systemSetup->setForceField(ui->forceField->currentData().toString());
    });
    systemSetup->setForceField(ui->forceField->currentData().toString());

    ui->removeHeteroAtoms->setChecked(systemSetup->getRemoveHeteroAtoms());
    connect(ui->removeHeteroAtoms, &QCheckBox::stateChanged, [this] (int state) {
        systemSetup->setRemoveHeteroAtoms(state == Qt::Checked);
    });

    connect(ui->pdbEntry, &QLineEdit::textChanged, [this] (const QString& pdbCode) {
        systemSetup->setPdbCode(pdbCode);
        if (pdbCode.length() == 4)
        {
            qDebug() << "Starting PDB download for" << pdbCode;
            auto* pdbDownloader = new PdbDownloader();
            connect(pdbDownloader, &PdbDownloader::downloaded,
                    [pdbDownloader, this] (const QString& /*pdbCode*/, const QString& file) {
                qDebug() << "donwloaded" << file;
                systemSetup->setSourceStructureFile(file);
                pdbDownloader->deleteLater();
            });

            // TODO does that belong here? Isn't that part of the project setup?
            QDir dir(systemSetup->getProject()->getProjectPath());
            if (dir.mkpath("input"))
            {
                dir.cd("input");
            }
            QFile file(dir.absolutePath() + "/" + pdbCode + ".pdb");
            QFileInfo fileInfo(file);
            pdbDownloader->download(pdbCode, fileInfo.absoluteFilePath());
        }
    });
    ui->pdbEntry->setText(systemSetup->getPdbCode());

    connect(systemSetup.get(), &SystemSetup::sourceStructureFileChanged,
        [this] (const QString& sourceStructureFile) {
        // Workaround since this callback is executed before the
        // callback that should update the molecule preview.
        QTimer::singleShot(100, [this, sourceStructureFile] {

            if (!sourceStructureFile.isEmpty())
            {
                ui->filterGroup->setEnabled(true);
                ui->systemSetupGroup->setEnabled(true);
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
