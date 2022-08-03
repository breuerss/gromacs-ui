#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include <QFileDialog>
#include <QDebug>
PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBrowseProjectDirectory, &QPushButton::clicked, this, &PreferencesDialog::openSelectProjectDirectory);
    connect(ui->buttonBrowseGmx, &QPushButton::clicked, this, &PreferencesDialog::openSelectPdb2gmx);
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
                QDir::homePath(),
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!newProjectDirectory.isEmpty())
    {
        setProjectDirectory(newProjectDirectory);
    }
}

void PreferencesDialog::openSelectPdb2gmx()
{
    const QString newGmxPath = QFileDialog::getOpenFileName(
                this,
                tr("Select Path to pdb2gmx"),
                QDir::homePath());

    if (!newGmxPath.isEmpty())
    {
        setGmx(newGmxPath);
    }
}

void PreferencesDialog::showEvent(QShowEvent *ev)
{
    QDialog::showEvent(ev);
    loadSettings();
}

void PreferencesDialog::saveSettings()
{
    settings.setValue(PROJECT_DIRECTORY, projectDirectory);
    settings.setValue(GMX_PATH, gmx);
}

void PreferencesDialog::loadSettings()
{
    setProjectDirectory(settings.value(PROJECT_DIRECTORY).toString());
    setGmx(settings.value(GMX_PATH).toString());
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
