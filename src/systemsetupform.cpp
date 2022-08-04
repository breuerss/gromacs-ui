#include "systemsetupform.h"
#include "ui_systemsetupform.h"
#include "pdbdownloader.h"
#include "pdbinfoextractor.h"
#include "project.h"
#include "gromacstoolexecutor.h"
#include <QDir>
#include <QCheckBox>
#include <QTimer>

SystemSetupForm::SystemSetupForm(std::shared_ptr<SystemSetup> newSystemSetup, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SystemSetupForm)
    , systemSetup(newSystemSetup)
{
    ui->setupUi(this);

    prepareWaterOptions();
    prepareForceFieldOptions();
    prepareBoxOptions();

    connect(systemSetup.get(), &SystemSetup::configReady, [this] () {
        qDebug() << "config ready";
        GromacsToolExecutor::execPdb2gmx(systemSetup);
        GromacsToolExecutor::execEditConf(systemSetup);
        GromacsToolExecutor::execSolvate(systemSetup);
    });

    connect(ui->boxType, QOverload<int>::of(&QComboBox::currentIndexChanged), [this] (int index) {
       systemSetup->setBoxType(ui->boxType->currentData().toString());
    });
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

    connect(ui->pdbEntry, &QLineEdit::textChanged, [this] (const QString& pdbCode) {
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

    connect(systemSetup.get(), &SystemSetup::sourceStructureFileChanged,
        [this] (const QString& sourceStructureFile) {
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
    static const QVariantMap map({
        { "SPC", "spc" },
        { "TIP3P", "tip3p" },
        { "TIP4P", "tip4p" }
    });

    for (const auto& label: map.keys())
    {
        ui->waterModel->addItem(label, map.value(label));
    }
    ui->waterModel->setCurrentIndex(1);
}

void SystemSetupForm::prepareForceFieldOptions()
{
    static const QVariantMap map({
        { "CHARMM27", "charmm27" },
        { "OPLS-AA/L", "oplsaa" }
    });

    for (const auto& label: map.keys())
    {
        ui->forceField->addItem(label, map.value(label));
    }
    ui->forceField->setCurrentIndex(0);
}

void SystemSetupForm::prepareBoxOptions()
{
    static const QVariantMap map({
        { "Cubic", "cubic" },
        { "Octahedron", "octahedron" },
        { "Dodecahedron", "dodecahedron" }
    });

    for (const auto& label: map.keys())
    {
        ui->boxType->addItem(label, map.value(label));
    }
    ui->boxType->setCurrentIndex(2);
}
