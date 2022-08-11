#include "systemsetupform.h"
#include "ui_systemsetupform.h"
#include "../pdbdownloader.h"
#include "../pdbinfoextractor.h"
#include "connectionhelper.h"
#include "../statusmessagesetter.h"
#include "../model/project.h"
#include "../model/systemsetup.h"
#include "../command/queue.h"
#include "../command/creategromacsmodel.h"
#include "../command/createbox.h"
#include "../command/solvate.h"
#include "../command/neutralise.h"

#include <QDir>
#include <QCheckBox>
#include <QTimer>

SystemSetupForm::SystemSetupForm(std::shared_ptr<Model::Project> newProject, QWidget *parent)
: QWidget(parent)
, ui(new Ui::SystemSetupForm)
, project(newProject)
  , systemSetup(project->getSystemSetup())
{
  ui->setupUi(this);

  prepareWaterOptions();
  prepareForceFieldOptions();
  prepareBoxOptions();
  setIonFromModel();
  connectIonSelectors();

  connectToLineEdit(ui->projectName, project, "name");
  projectConn = connect(project.get(), &Model::Project::nameChanged, [this] (const QString& projectName) {
    ui->systemCoordinatesGroup->setEnabled(!projectName.isEmpty());
  });
  ui->systemCoordinatesGroup->setEnabled(!project->property("name").toString().isEmpty());

  QRegExpValidator* projectNameValidator = new QRegExpValidator(QRegExp("[a-zA-Z0-9_]+"));
  ui->projectName->setValidator(projectNameValidator);

  auto queue = std::make_shared<Command::Queue>();
  connect(queue.get(), &Command::Queue::finished, [this] (bool success) {
    systemSetup->setStructureReady(success);
  });
  connect(systemSetup.get(), &Model::SystemSetup::configReadyChanged, [this, queue] (bool ready) {
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
    ui->pdbEntry->setStyleSheet("");
    if (pdbCode.length() == 4)
    {
      // TODO does that belong here? Isn't that part of the project setup?
      QDir dir(project->getProjectPath());
      if (dir.mkpath("input"))
      {
        dir.cd("input");
      }
      QString absFilePath = dir.absolutePath() + "/" + pdbCode + ".pdb";
      handlePdbDownload(pdbCode, absFilePath);
    }
    else
    {
      setGroupsEnabled(false);
    }
  });

  connect(systemSetup.get(), &Model::SystemSetup::sourceStructureFileChanged,
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
  disconnect(projectConn);
  delete ui;
}

void SystemSetupForm::prepareWaterOptions()
{
  using Model::SystemSetup;
  setOptions<SystemSetup::WaterModel>(
    ui->waterModel,
    {
      //{ "None", SystemSetup::WaterModel::None }, // not yet properly handled
      { "SPC", SystemSetup::WaterModel::SPC },
      { "TIP3P", SystemSetup::WaterModel::TIP3P },
      { "TIP4P", SystemSetup::WaterModel::TIP4P },
      { "TIP5P", SystemSetup::WaterModel::TIP5P },
    }, SystemSetup::WaterModel::SPC);
}

void SystemSetupForm::prepareForceFieldOptions()
{
  using Model::SystemSetup;
  setOptions<SystemSetup::ForceField>(
    ui->forceField,
    {
      { "CHARMM27", SystemSetup::ForceField::CHARMM27 },
      { "OPLS-AA/L", SystemSetup::ForceField::OPLSAA }
  }, SystemSetup::ForceField::CHARMM27);
}

void SystemSetupForm::prepareBoxOptions()
{
  using Model::SystemSetup;
  setOptions<SystemSetup::BoxType>(
    ui->boxType,
    {
      { "Cubic", SystemSetup::BoxType::Cubic },
      { "Octahedron", SystemSetup::BoxType::Octahedron },
      { "Dodecahedron", SystemSetup::BoxType::Dodecahedron }
    },
    SystemSetup::BoxType::Dodecahedron);
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

void SystemSetupForm::handlePdbDownload(const QString& pdbCode, const QString& absFilePath)
{
  auto* pdbDownloader = new PdbDownloader();
  connect(pdbDownloader, &PdbDownloader::downloaded,
          [pdbDownloader, this] (const QString& /*pdbCode*/, const QString& file) {
            pdbDownloader->deleteLater();
            systemSetup->setSourceStructureFile(file);
          });
  connect(pdbDownloader, &PdbDownloader::notFound, [pdbCode, pdbDownloader, this] ()
          {
            pdbDownloader->deleteLater();
            ui->pdbEntry->setStyleSheet("background: #55990000");
            StatusMessageSetter::getInstance()->setMessage(tr("PDB with id '%1' not found.").arg(pdbCode));
          });
  connect(pdbDownloader, &PdbDownloader::error, [pdbDownloader] ()
          {
            pdbDownloader->deleteLater();
            StatusMessageSetter::getInstance()->setMessage(tr("Error communicating with PDB service."));
          });

  pdbDownloader->download(pdbCode, absFilePath);

}
