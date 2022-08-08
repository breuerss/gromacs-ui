#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "preferencesdialog.h"
#include "projectmanager.h"
#include "fileserver.h"
#include "systemsetupform.h"
#include "simulationsetupform.h"
#include "statusmessagesetter.h"
#include "logforwarder.h"

#include "command/queue.h"
#include "command/runsimulation.h"

#include "model/project.h"
#include "model/step.h"
#include "model/systemsetup.h"

#include <QDebug>
#include <QWebEngineSettings>
#include <QUrlQuery>

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

    auto queue = std::make_shared<Command::Queue>();
    auto project = ProjectManager::getInstance()->getCurrentProject();
    connect (project.get(), &Model::Project::stepRemoved,
        [this] (std::shared_ptr<Model::Step>, int at) {
        removeTabAt(at + 1);
    });

    connect(project.get(), &Model::Project::nameChanged, [this, project] (const QString& newName) {
        bool canContinue = !newName.isEmpty();
        ui->actionAddStep->setEnabled(canContinue);
        ui->actionCreateDefaultSimulationSetup->setEnabled(canContinue);
        ui->actionRunSimulation->setEnabled(canContinue && !project->getSystemSetup()->getNeutralisedStructureFile().isEmpty());
    });
    connect(LogForwarder::getInstance(), &LogForwarder::addMessage, ui->logOutput,
            &QPlainTextEdit::appendPlainText);
    connect(queue.get(), &Command::Queue::stepFinished, [this, project] (int stepIndex, bool success) {
        if (success)
        {
            QString projectPath = project->getProjectPath();
            QString stepType = project->getSteps()[stepIndex]->getDirectory();
            QString basePath = projectPath + "/" + stepType + "/" + stepType;
            QString trajectory = basePath + ".xtc";
            if (!QFile(trajectory).exists())
            {
                trajectory = "";
            }

            qDebug() << "molecule files" << basePath + ".gro" << trajectory;
            setMoleculeFile(basePath + ".gro", trajectory);
        }
    });
    connect(ui->actionRunSimulation, &QAction::triggered, [queue, project] () {
        queue->clear();
        int noOfSteps = project->getSteps().size();
        for (int stepIndex = 0; stepIndex < noOfSteps; ++stepIndex)
        {
            queue
                ->enqueue(std::make_shared<Command::RunSimulation>(project, stepIndex));
        }
        queue->start();
    });

    connect(StatusMessageSetter::getInstance(), &StatusMessageSetter::messageChanged,
        [this] (const QString& message) {
        ui->statusbar->showMessage(message, 10000);
    });
    setupUIForProject();
    connect(ProjectManager::getInstance(), &ProjectManager::currentProjectChanged, [this] () {
        connect(ProjectManager::getInstance()->getCurrentProject().get(),
            &Model::Project::stepAdded, this, &MainWindow::addTabForStep);
        setupUIForProject();
    });
    connect(ProjectManager::getInstance()->getCurrentProject().get(), &Model::Project::stepAdded,
        this, &MainWindow::addTabForStep);

    connect(ui->stepconfigurator, &QTabWidget::tabCloseRequested,
            [] (int index) {
        ProjectManager::getInstance()->getCurrentProject()->removeStep(index);
    });

    connect(ui->actionCreateDefaultSimulationSetup, &QAction::triggered,
            [] () {

        auto* manager = ProjectManager::getInstance();
        auto project = manager->getCurrentProject();
        project->clearSteps();
        auto step = project->addStep();
        step->setProperty("simulationType", QVariant::fromValue(Model::SimulationType::Minimisation));
        step->setProperty("algorithm", "steep");
        step = project->addStep();
        step->setProperty("simulationType", QVariant::fromValue(Model::SimulationType::NVT));
        step->setProperty("numberOfSteps", 10000);
        step->setProperty("algorithm", "md");

        manager->currentProjectChanged(project);
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

        while (ui->stepconfigurator->count())
        {
            removeTabAt(0);
        }
        ui->stepconfigurator->addTab(new SystemSetupForm(project), tr("System Setup"));
        auto* tabBar = ui->stepconfigurator->tabBar();
        tabBar->tabButton(0, QTabBar::RightSide)->deleteLater();
        tabBar->setTabButton(0, QTabBar::RightSide, 0);
        for (auto step: project->getSteps())
        {
            addTabForStep(step);
        }

        Model::SystemSetup* systemSetup = project->getSystemSetup().get();
        connect(systemSetup, &Model::SystemSetup::structureReadyChanged,
            ui->actionRunSimulation, &QAction::setEnabled);
        connect(systemSetup, &Model::SystemSetup::structureReadyChanged,
            [this] (bool ready) {
                int tabIndex = 0;
                if (!ready)
                {
                    tabIndex = 1;
                    ui->logOutput->setPlainText("");
                }
                ui->tabWidget->setCurrentIndex(tabIndex);
            });
        ui->actionRunSimulation->setEnabled(project->getSystemSetup()->getStructureReady());

        connect(systemSetup, &Model::SystemSetup::sourceStructureFileChanged,
                [this] (const QString& fileName) {
            setMoleculeFile(fileName);
        });

        connect(systemSetup, &Model::SystemSetup::filteredStructureFileChanged,
                [this] (const QString& fileName) {
            setMoleculeFile(fileName);
        });

        connect(systemSetup, &Model::SystemSetup::solvatedStructureFileChanged,
                [this] (const QString& fileName) {
            setMoleculeFile(fileName);
        });

        connect(systemSetup, &Model::SystemSetup::neutralisedStructureFileChanged,
                [this] (const QString& fileName) {
            setMoleculeFile(fileName);
        });

        QWebEngineSettings* settings = ui->molpreview->page()->settings();
        settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    }

    setMoleculeFile();
}

void MainWindow::addTabForStep(std::shared_ptr<Model::Step> step, int at)
{
    if (at == -1)
    {
        at = ui->stepconfigurator->count();
    }
    connect(step.get(), &Model::Step::simulationTypeChanged, [this, step, at] () {
        ui->stepconfigurator->setTabText(at, step->getName());
    });
    ui->stepconfigurator->insertTab(at, new SimulationSetupForm(step), step->getName());
}

void MainWindow::setMoleculeFile(const QString& file, const QString& trajectory)
{
    // TODO shared path from install target
    QString currentDir = QCoreApplication::applicationDirPath();
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

void MainWindow::removeTabAt(int index)
{
    QWidget* widget = ui->stepconfigurator->widget(index);
    ui->stepconfigurator->removeTab(index);
    delete widget;
}
