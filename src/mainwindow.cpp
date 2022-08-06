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
#include "gromacstoolexecutor.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , preferencesDialog(new PreferencesDialog(this))
{
    ui->setupUi(this);
    setGeometry(settings.value(Settings::APP_GEOMETRY, QRect(0, 0, 800, 600)).toRect());

    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::openPreferencesDialog);
    connect(ui->actionNewProject, &QAction::triggered,
        ProjectManager::getInstance(), &ProjectManager::createNewProject);
    connect(ui->actionAddStep, &QAction::triggered, [] () {
        ProjectManager::getInstance()->getCurrentProject()->addStep();
    });
    connect(ui->actionSave, &QAction::triggered,
        ProjectManager::getInstance(), &ProjectManager::save);
    connect(ui->actionSaveProjectAs, &QAction::triggered, [] () {
        ProjectManager::getInstance()->saveAs();
    });
    connect(ui->actionOpenProject, &QAction::triggered,
        ProjectManager::getInstance(), &ProjectManager::open);

    connect(ui->actionRunSimulation, &QAction::triggered, [this] () {
        std::shared_ptr<Project> project = ProjectManager::getInstance()->getCurrentProject();
        const auto& steps = project->getSteps();
        int noOfSteps = steps.size();
        QString projectPath = project->getProjectPath();
        for (int stepIndex = 0; stepIndex < noOfSteps; ++stepIndex)
        {
            GromacsToolExecutor::execMdrun(project, stepIndex);
            QString stepType = steps[stepIndex]->getDirectory();
            QString basePath = projectPath + "/" + stepType + "/" + stepType;
            QString trajectory = basePath + ".xtc";
            if (!QFile(trajectory).exists())
            {
                trajectory = "";
            }

            setMoleculeFile(basePath + ".gro", trajectory);
        }
    });

    connect(StatusMessageSetter::getInstance(), &StatusMessageSetter::messageChanged,
        [this] (const QString& message) {
        ui->statusbar->showMessage(message, 10000);
    });
    setupUIForProject();
    connect(ProjectManager::getInstance(), &ProjectManager::currentProjectChanged, [this] () {
        connect(ProjectManager::getInstance()->getCurrentProject().get(), &Project::stepAdded,
            this, &MainWindow::addTabForStep);
        setupUIForProject();
    });
    connect(ProjectManager::getInstance()->getCurrentProject().get(), &Project::stepAdded,
        this, &MainWindow::addTabForStep);

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
        for (auto step: project->getSteps())
        {
            addTabForStep(step);
        }

        connect(project->getSystemSetup().get(), &SystemSetup::sourceStructureFileChanged,
                [this] (const QString& fileName) {
            setMoleculeFile(fileName);
        });

        connect(project->getSystemSetup().get(), &SystemSetup::filteredStructureFileChanged,
                [this] (const QString& fileName) {
            setMoleculeFile(fileName);
        });

        connect(project->getSystemSetup().get(), &SystemSetup::solvatedStructureFileChanged,
                [this] (const QString& fileName) {
            setMoleculeFile(fileName);
        });

        auto* settings = ui->molpreview->page()->settings();
        settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    }

    setMoleculeFile();
}

void MainWindow::addTabForStep(std::shared_ptr<Step> step, int at)
{
    if (at == -1)
    {
        at = ui->stepconfigurator->count();
    }
    ui->stepconfigurator->insertTab(at, new SimulationSetupForm(step), step->getName());
}

void MainWindow::setMoleculeFile(const QString& file, const QString& trajectory)
{
    // TODO shared path from install target
    QString currentDir = QDir::currentPath();
    QUrl url = QUrl::fromLocalFile(currentDir + "/../embedded.html");

    if (!file.isEmpty())
    {
        QString modelUrl = FileServer::getInstance()->getUrlForFile(file);
        QString trajectoryUrl = trajectory;
        if (!trajectory.isEmpty())
        {
            trajectoryUrl = FileServer::getInstance()->getUrlForFile(trajectory);
        }
        url.setQuery(QUrlQuery({
                                   { "structure", modelUrl },
                                   { "trajectory", trajectoryUrl }
                               }));
    }

    ui->molpreview->setUrl(url);
}
