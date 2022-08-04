#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "preferencesdialog.h"
#include "projectmanager.h"
#include <QDir>
#include <QDebug>
#include <QWebEngineSettings>
#include <QUrlQuery>
#include <QWebEngineProfile>
#include "fileserver.h"
#include "systemsetupform.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , preferencesDialog(new PreferencesDialog(this))
{
    ui->setupUi(this);
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::openPreferencesDialog);
    connect(ui->actionNewProject, &QAction::triggered, [] () {
       ProjectManager::getInstance()->createNewProject();
    });
    setGeometry(settings.value(Settings::APP_GEOMETRY, QRect(0, 0, 800, 600)).toRect());
    setupUIForProject();
    connect(ProjectManager::getInstance(), &ProjectManager::currentProjectChanged, this, &MainWindow::setupUIForProject);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openPreferencesDialog()
{
    preferencesDialog->exec();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    settings.setValue(Settings::APP_GEOMETRY, geometry());
    QMainWindow::closeEvent(event);
}

void MainWindow::setupUIForProject()
{
    auto project = ProjectManager::getInstance()->getCurrentProject();

    if (project)
    {
        ui->stepconfigurator->setEnabled(true);
        ui->stepconfigurator->clear();
        ui->stepconfigurator->addTab(new SystemSetupForm(project->getSystemSetup()), tr("System Setup"));
        for (auto& step: project->getSteps())
        {
            (void) step;
        }

        connect(project->getSystemSetup().get(), &SystemSetup::sourceStructureFileChanged,
                this, &MainWindow::setMoleculeFile);

        connect(project->getSystemSetup().get(), &SystemSetup::processedStructureFileChanged,
                this, &MainWindow::setMoleculeFile);

        auto* settings = ui->molpreview->page()->settings();
        settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
        //settings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    }

    setMoleculeFile();
}

void MainWindow::setMoleculeFile(const QString& file)
{
    // TODO shared path
    QString currentDir = QDir::currentPath();
    QUrl url = QUrl::fromLocalFile(currentDir + "/../embedded.html");

    if (!file.isEmpty())
    {
        QString modelUrl = FileServer::getInstance()->getUrlForFile(file);
        qDebug() << modelUrl;
        url.setQuery(QUrlQuery({{ "file", modelUrl }}));
    }

    qDebug() << url;
    ui->molpreview->setUrl(url);
}
