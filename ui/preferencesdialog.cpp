#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
PreferencesDialog::PreferencesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PreferencesDialog)
{
  ui->setupUi(this);
  connect(ui->buttonBrowseProjectDirectory, &QPushButton::clicked, this, &PreferencesDialog::openSelectProjectDirectory);
  connect(ui->buttonBrowseGmx, &QPushButton::clicked, this, &PreferencesDialog::openSelectGmx);
  connect(ui->buttonBrowsePdbwc, &QPushButton::clicked, this, &PreferencesDialog::openSelectPdbwc);
  connect(ui->buttonBrowsePdbchain, &QPushButton::clicked, this, &PreferencesDialog::openSelectPdbchain);
  connect(ui->buttonBrowsePdbdelhetatm, &QPushButton::clicked, this, &PreferencesDialog::openSelectPdbdelhetatm);
  // TODO react to changes in text fields in preferences dialog
  connect(this, &QDialog::accepted, this, &PreferencesDialog::saveSettings);
}

PreferencesDialog::~PreferencesDialog()
{
  delete ui;
}

void PreferencesDialog::openSelectProjectDirectory()
{
  const QString newProjectDirectory = QFileDialog::getExistingDirectory(
    this,
    tr("Select Project Directory"),
    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  if (!newProjectDirectory.isEmpty())
  {
    setProjectDirectory(newProjectDirectory);
  }
}

void PreferencesDialog::openSelectGmx()
{
  const QString newGmxPath = QFileDialog::getOpenFileName(
    this,
    tr("Select Path to gmx"),
    QCoreApplication::applicationDirPath());

  if (!newGmxPath.isEmpty())
  {
    setGmx(newGmxPath);
  }
}

void PreferencesDialog::openSelectPdbwc()
{
  const QString newPdbwcPath = QFileDialog::getOpenFileName(
    this,
    tr("Select Path to pdb_wc"),
    QCoreApplication::applicationDirPath());

  if (!newPdbwcPath.isEmpty())
  {
    setPdbwc(newPdbwcPath);
  }
}

void PreferencesDialog::openSelectPdbchain()
{
  const QString newPdbchainPath = QFileDialog::getOpenFileName(
    this,
    tr("Select Path to pdb_chain"),
    QCoreApplication::applicationDirPath());

  if (!newPdbchainPath.isEmpty())
  {
    setPdbchain(newPdbchainPath);
  }
}

void PreferencesDialog::openSelectPdbdelhetatm()
{
  const QString newPdbdelhetatmPath = QFileDialog::getOpenFileName(
    this,
    tr("Select Path to pdb_delhetatm"),
    QCoreApplication::applicationDirPath());

  if (!newPdbdelhetatmPath.isEmpty())
  {
    setPdbdelhetatm(newPdbdelhetatmPath);
  }
}

void PreferencesDialog::showEvent(QShowEvent *ev)
{
  QDialog::showEvent(ev);
  loadSettings();
}

void PreferencesDialog::saveSettings()
{
  settings.setValue(Settings::PROJECTS_DIRECTORY, projectDirectory);
  settings.setValue(Settings::GMX_PATH, gmx);
  settings.setValue(Settings::PDB_WC, pdbwc);
  settings.setValue(Settings::PDB_SELCHAIN, pdbchain);
  settings.setValue(Settings::PDB_DELHETATM, pdbdelhetatm);
}

void PreferencesDialog::loadSettings()
{
  setProjectDirectory(settings.value(Settings::PROJECTS_DIRECTORY).toString());
  setGmx(settings.value(Settings::GMX_PATH).toString());
  setPdbwc(settings.value(Settings::PDB_WC).toString());
  setPdbchain(settings.value(Settings::PDB_SELCHAIN).toString());
  setPdbdelhetatm(settings.value(Settings::PDB_DELHETATM).toString());
}

void PreferencesDialog::setProjectDirectory(const QString &newProjectDirectory)
{
  projectDirectory = newProjectDirectory;
  ui->projectDirectory->setText(projectDirectory);
}

void PreferencesDialog::setGmx(const QString &newGmxPath)
{
  gmx = newGmxPath;
  ui->gmx->setText(gmx);
}

void PreferencesDialog::setPdbwc(const QString &newPdbwc)
{
  pdbwc = newPdbwc;
  ui->pdbwc->setText(pdbwc);
}

void PreferencesDialog::setPdbchain(const QString &newPdbchain)
{
  pdbchain = newPdbchain;
  ui->pdbchain->setText(pdbchain);
}

void PreferencesDialog::setPdbdelhetatm(const QString &newPdbdelhetatm)
{
  pdbdelhetatm = newPdbdelhetatm;
  ui->pdbdelhetatm->setText(pdbdelhetatm);
}
