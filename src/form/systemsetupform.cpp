#include "../appprovider.h"
#include "src/command/executor.h"
#include "src/command/inputfilelink.h"
#include "src/command/inputoutputfilelink.h"
#include "systemsetupform.h"
#include "ui_systemsetupform.h"
#include "../pdbdownloader.h"
#include "../pdbinfoextractor.h"
#include "connectionhelper.h"
#include "../statusmessagesetter.h"
#include "../model/project.h"
#include "../model/systemsetup.h"
#include "../command/queue.h"
#include "../command/solvate.h"
#include "../command/neutralise.h"
#include "../command/filter.h"

#include <QDir>
#include <QCheckBox>
#include <QTimer>
#include <QFileDialog>
#include <QStandardPaths>
#include <memory>

SystemSetupForm::SystemSetupForm(std::shared_ptr<Model::Project> newProject, QWidget *parent)
: QWidget(parent)
, ui(new Ui::SystemSetupForm)
, project(newProject)
  , systemSetup(project->getSystemSetup())
  , queue(std::make_shared<Command::Queue>())
{
  ui->setupUi(this);

  setIonFromModel();
  connectIonSelectors();

  connectToLineEdit(ui->projectName, project, "name");
  conns << connect(project.get(), &Model::Project::nameChanged, [this] (const QString& projectName) {
    ui->systemCoordinatesGroup->setEnabled(!projectName.isEmpty());
  });
  ui->systemCoordinatesGroup->setEnabled(!project->property("name").toString().isEmpty());

  QRegExpValidator* projectNameValidator = new QRegExpValidator(QRegExp("[a-zA-Z0-9_]+"));
  ui->projectName->setValidator(projectNameValidator);

  connect(queue.get(), &Command::Queue::finished, [this] (bool success) {
    ui->currentlyRunning->setText("");
    ui->processingProgress->setEnabled(false);
    if (success)
    {
      auto link = std::dynamic_pointer_cast<Command::InputOutputFileLink>(queue->last());
      if (link)
      {
        systemSetup->setProcessedStructureFile(link->getOutputFilename());
      }
    }
  });
  connect(queue.get(), &Command::Queue::stepStarted,
          [this] (size_t, std::shared_ptr<Command::Executor> step) {
            ui->currentlyRunning->setText(step->getName());
          });

  connect(queue.get(), &Command::Queue::stepFinished,
          [this] (size_t stepIndex, std::shared_ptr<Command::Executor>, bool) {
    ui->processingProgress->setValue(100.0 * (stepIndex + 1)/ queue->getSize());
  });


  connectToComboBox<QString>(ui->boxType, systemSetup, "boxType");
  connectToComboBox<QString>(ui->waterModel, systemSetup, "waterModel");
  connectToComboBox<QString>(ui->forceField, systemSetup, "forceField");

  connectToSpinBox<QDoubleSpinBox, double>(ui->ionConcentration, systemSetup, "ionConcentration");

  connectToCheckbox(ui->removeHeteroAtoms, systemSetup, "removeHeteroAtoms");

  auto reactToSourceStructureFileChange = [this] (const QString& sourceStructureFile) {
    if (!sourceStructureFile.isEmpty())
    {
      setGroupsEnabled(true);
      const bool isPdb = sourceStructureFile.endsWith(".pdb");
      setPdbfixerEnabled(isPdb);
      setFilterEnabled(isPdb);
      if (isPdb)
      {
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
    }
  };
  conns << connect(
    systemSetup.get(),
    &Model::SystemSetup::sourceStructureFileChanged,
    reactToSourceStructureFileChange
    );

  setGroupsEnabled(!systemSetup->getSourceStructureFile().isEmpty());
  connect(ui->generateInputCoordinates, &QPushButton::clicked,
          [this] () {
            QString icon = "system-run";
            QString text = tr("Process Coordinates");
            if (queue->isRunning())
            {
              stopPreprocess();
            }
            else
            {
              icon = "media-playback-stop";
              text = tr("Stop processing");
              preprocess();
            }
            ui->generateInputCoordinates->setIcon(QIcon::fromTheme(icon));
            ui->generateInputCoordinates->setText(text);
          });

  auto reactToPdbCode = [this] (const QString& pdbCode) {
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
  };
  connectToLineEdit(ui->pdbEntry, systemSetup, "pdbCode", reactToPdbCode);
  qDebug() << systemSetup->property("pdbCode");
  reactToPdbCode(systemSetup->property("pdbCode").toString());

  setPdbfixerEnabled(!AppProvider::get("pdbfixer").isEmpty());

  connect(ui->localFileDialogButton, &QPushButton::clicked,
          this, &SystemSetupForm::loadFileFromDisk);
}

SystemSetupForm::~SystemSetupForm()
{
  for (auto conn : conns)
  {
    disconnect(conn);
  }
  delete ui;
}

void SystemSetupForm::setPdbfixerEnabled(bool enabled)
{
  ui->usePdbFixer->setEnabled(enabled);
  ui->usePdbFixer->setChecked(enabled);
}

void SystemSetupForm::setFilterEnabled(bool enabled)
{
  ui->useFilter->setEnabled(enabled);
  ui->useFilter->setChecked(enabled);
}

void SystemSetupForm::preprocess()
{
  qDebug() << "preprocess";
  queue->clear();
  ui->processingProgress->setEnabled(true);
  ui->processingProgress->setValue(0);

  if (ui->useFilter->isChecked())
  {
    queue->enqueue(std::make_shared<Command::Filter>(systemSetup));
  }

  //queue->enqueue(std::make_shared<Command::CreateGromacsModel>(systemSetup));

  if (ui->usecreateBox->isChecked())
  {
    //queue->enqueue(std::make_shared<Command::CreateBox>(systemSetup));
  }

  if (ui->useSolvate->isChecked())
  {
    //queue->enqueue(std::make_shared<Command::Solvate>(systemSetup));
  }

  if (ui->useNeutralise->isChecked() && ui->useNeutralise->isEnabled())
  {
    queue->enqueue(std::make_shared<Command::Neutralise>(systemSetup));
  }

  auto firstCommand = queue->first();
  auto link = dynamic_cast<Command::InputOutputFileLink*>(firstCommand.get());

  if (link)
  {
    auto inputLink = std::make_shared<Command::InputFileLink>(systemSetup->getSourceStructureFile());
    link->setPreviousLink(inputLink);
  }

  queue->start();
}

void SystemSetupForm::stopPreprocess()
{
  queue->stop();
  ui->processingProgress->setEnabled(false);
}

void SystemSetupForm::showEvent(QShowEvent*)
{
  ui->projectName->setFocus();
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
  for (auto button: positiveIonMap.keys())
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
  for (auto button: negativeIonMap.keys())
  {
    QString ion = negativeIonMap[button];
    connect(
      button,
      &QRadioButton::toggled,
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
  ui->preprocessingGroup->setEnabled(enabled);
  ui->simulationModelGroup->setEnabled(enabled);
  ui->generateInputCoordinates->setEnabled(enabled);
}

void SystemSetupForm::handlePdbDownload(const QString& pdbCode, const QString& absFilePath)
{
  auto pdbDownloader = new PdbDownloader();
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

void SystemSetupForm::loadFileFromDisk()
{
  QString fileName = QFileDialog::getOpenFileName(
    nullptr,
    tr("Load structure from file"),
    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
    tr("Structures (*.gro *.pdb)")
    );
  if (!fileName.isEmpty())
  {
    systemSetup->setSourceStructureFile(fileName);
  }
}
