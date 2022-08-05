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
#include "simulationsetupform.h"
#include "statusmessagesetter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , preferencesDialog(new PreferencesDialog(this))
{
    ui->setupUi(this);
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::openPreferencesDialog);
    connect(ui->actionNewProject, &QAction::triggered,
       ProjectManager::getInstance(), &ProjectManager::createNewProject);
    connect(ui->actionAddStep, &QAction::triggered,
       ProjectManager::getInstance(), &ProjectManager::addStep);

    connect(ui->actionSave, &QAction::triggered,
            ProjectManager::getInstance(), &ProjectManager::save);
    connect(ui->actionSave_Project_As, &QAction::triggered, [] () {
            ProjectManager::getInstance()->saveAs();
    });
    connect(ui->actionOpen_Project, &QAction::triggered,
            ProjectManager::getInstance(), &ProjectManager::open);
    connect(StatusMessageSetter::getInstance(), &StatusMessageSetter::messageChanged,
        [this] (const QString& message) {
        ui->statusbar->showMessage(message, 10000);
    });
    setGeometry(settings.value(Settings::APP_GEOMETRY, QRect(0, 0, 800, 600)).toRect());
    setupUIForProject();
    connect(ProjectManager::getInstance(), &ProjectManager::currentProjectChanged, this, &MainWindow::setupUIForProject);
    connect(ui->stepconfigurator, &QTabWidget::tabCloseRequested,
            [this] (int index) {
        ProjectManager::getInstance()->getCurrentProject()->removeStep(index);
        ui->stepconfigurator->removeTab(index);
    });
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
        auto* tabBar = ui->stepconfigurator->tabBar();
        tabBar->tabButton(0, QTabBar::RightSide)->deleteLater();
        tabBar->setTabButton(0, QTabBar::RightSide, 0);
        for (auto& step: project->getSteps())
        {
            ui->stepconfigurator->addTab(new SimulationSetupForm(step), tr("Simulation Step"));
        }

        connect(project->getSystemSetup().get(), &SystemSetup::sourceStructureFileChanged,
                this, &MainWindow::setMoleculeFile);

        connect(project->getSystemSetup().get(), &SystemSetup::filteredStructureFileChanged,
                this, &MainWindow::setMoleculeFile);

        connect(project->getSystemSetup().get(), &SystemSetup::solvatedStructureFileChanged,
                this, &MainWindow::setMoleculeFile);

        auto* settings = ui->molpreview->page()->settings();
        settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    }

    setMoleculeFile();
}

void MainWindow::setMoleculeFile(const QString& file)
{
    // TODO shared path from install target
    QString currentDir = QDir::currentPath();
    QUrl url = QUrl::fromLocalFile(currentDir + "/../embedded.html");

    if (!file.isEmpty())
    {
        QString modelUrl = FileServer::getInstance()->getUrlForFile(file);
        url.setQuery(QUrlQuery({{ "file", modelUrl }}));
    }

    ui->molpreview->setUrl(url);
}
