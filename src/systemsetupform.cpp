#include "systemsetupform.h"
#include "ui_systemsetupform.h"
#include "pdbdownloader.h"
#include "pdbinfoextractor.h"
#include <QDir>
#include <QCheckBox>

SystemSetupForm::SystemSetupForm(std::shared_ptr<SystemSetup> newSystemSetup, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SystemSetupForm)
    , systemSetup(newSystemSetup)
{
    ui->setupUi(this);

    prepareWaterOptions();
    prepareForceFieldOptions();
    prepareBoxOptions();

    connect(ui->pdbEntry, &QLineEdit::textChanged, [this] (const QString& text) {
        if (text.length() == 4)
        {
            qDebug() << "Starting PDB download for" << text;
            auto* pdbDownloader = new PdbDownloader();
            connect(pdbDownloader, &PdbDownloader::downloaded,
                    [pdbDownloader, this] (const QString& pdbCode, const QString& content) {
                // TODO make that reusable
                QDir dir(systemSetup->getProject()->getProjectPath());

                if (dir.mkpath("input"))
                {
                    dir.cd("input");
                }
                QFile file(dir.absolutePath() + "/" + pdbCode + ".pdb");
                file.open(QFile::WriteOnly);
                file.write(content.toUtf8());
                file.close();
                QFileInfo fileInfo(file);
                systemSetup->setSourceStructureFile(fileInfo.absoluteFilePath());
                pdbDownloader->deleteLater();
            });
            pdbDownloader->download(text);
        }
    });

    connect(systemSetup.get(), &SystemSetup::sourceStructureFileChanged,
        [this] (const QString& sourceStructureFile) {
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

                for (const auto& chain: chains)
                {
                    QCheckBox* checkBox = new QCheckBox(chain);
                    systemSetup->useChain(chain);
                    checkBox->setCheckState(Qt::Checked);
                    connect(checkBox, &QCheckBox::stateChanged, [this, chain] (int state) {
                         systemSetup->useChain(chain, state == Qt::Checked);
                    });
                    ui->chainsGroupLayout->addWidget(checkBox);
                }
            }
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
}
