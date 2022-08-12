#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "form/preferencesdialog.h"
#include "projectmanager.h"
#include "form/systemsetupform.h"
#include "form/simulationsetupform.h"
#include "statusmessagesetter.h"
#include "simulationstatuschecker.h"
#include "logforwarder.h"

#include "command/queue.h"
#include "command/runsimulation.h"

#include "model/project.h"
#include "model/simulation.h"
#include "model/systemsetup.h"

#include <QDebug>
#include <QWebEngineSettings>
#include <QUrlQuery>
#include <QDir>
#include <QToolButton>
#include <QCoreApplication>
#include <climits>

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
  , preferencesDialog(new PreferencesDialog(this))
  , queue(std::make_shared<Command::Queue>())
{
  ui->setupUi(this);
  setGeometry(settings.value(Settings::APP_GEOMETRY, QRect(0, 0, 800, 600)).toRect());
  ui->splitter->setSizes({INT_MAX, INT_MAX});

  connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::openPreferencesDialog);
  connect(ui->actionNewProject, &QAction::triggered,
          ProjectManager::getInstance(), &ProjectManager::createNewProject);
  connect(ui->actionAddStep, &QAction::triggered, [] () {
    ProjectManager::getInstance()->getCurrentProject()->addStep();
  });
  connect(ui->actionSave, &QAction::triggered,
          ProjectManager::getInstance(), &ProjectManager::save);
  connect(ui->actionSaveProjectAs, &QAction::triggered,
          ProjectManager::getInstance(), QOverload<>::of(&ProjectManager::saveAs));
  connect(ui->actionOpenProject, &QAction::triggered,
          ProjectManager::getInstance(), &ProjectManager::open);
  connect(ui->actionRunSimulation, &QAction::triggered,
          queue.get(), &Command::Queue::start);

  // add button to add step to tab widget
  QToolButton* addStepButton = new QToolButton(this);
  addStepButton->setCursor(Qt::ArrowCursor);
  addStepButton->setAutoRaise(true);
  addStepButton->setIcon(QIcon::fromTheme("add"));
  connect(addStepButton, &QToolButton::clicked, ui->actionAddStep, &QAction::trigger);
  ui->stepconfigurator->setCornerWidget(addStepButton);

  connect(LogForwarder::getInstance(), &LogForwarder::addMessage,
          ui->logOutput, &QPlainTextEdit::appendPlainText);

  connect(queue.get(), &Command::Queue::stepFinished, [this] (int stepIndex, bool success) {
    if (success)
    {
      auto project = ProjectManager::getInstance()->getCurrentProject();
      SimulationStatusChecker checker(project, project->getSteps()[stepIndex]);

      QString trajectory;
      if (checker.hasTrajectory())
      {
        trajectory = checker.getTrajectoryPath();
      }

      QString coordinates = checker.getCoordinatesPath();
      setMoleculeFile(coordinates, trajectory);
    }
  });

  connect(StatusMessageSetter::getInstance(), &StatusMessageSetter::messageChanged,
          [this] (const QString& message) {
            ui->statusbar->showMessage(message);
          });

  setupUIForProject();
  connect(ProjectManager::getInstance(), &ProjectManager::currentProjectChanged,
          this, &MainWindow::setupUIForProject);

  connect(ui->stepconfigurator, &QTabWidget::tabCloseRequested,
          [] (int index) {
            ProjectManager::getInstance()->getCurrentProject()->removeStep(index - 1);
          });

  connect(
    ui->actionCreateDefaultSimulationSetup,
    &QAction::triggered,
    [] () {

      using Model::Simulation;
      auto* manager = ProjectManager::getInstance();
      auto project = manager->getCurrentProject();
      project->clearSteps();
      auto step = project->addStep();
      step->setProperty("simulationType", QVariant::fromValue(Simulation::Type::Minimisation));
      step->setProperty("algorithm", "steep");
      step = project->addStep();
      step->setProperty("simulationType", QVariant::fromValue(Simulation::Type::NVT));
      step->setProperty("numberOfSteps", 10000);
      step->setProperty("algorithm", "md");
      step = project->addStep();
      step->setProperty("simulationType", QVariant::fromValue(Simulation::Type::NPT));
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
    for (auto conn : conns)
    {
      disconnect(conn);
    }

    conns << connect(project.get(),
            &Model::Project::stepAdded, this, &MainWindow::addTabForStep);
    Model::SystemSetup* systemSetup = project->getSystemSetup().get();
    conns << connect(systemSetup, &Model::SystemSetup::structureReadyChanged,
            ui->actionRunSimulation, &QAction::setEnabled);
    conns << connect(systemSetup, &Model::SystemSetup::structureReadyChanged, [this] (bool ready) {
      int tabIndex = 0;
      if (!ready)
      {
        tabIndex = 1;
        ui->logOutput->setPlainText("");
      }
      ui->tabWidget->setCurrentIndex(tabIndex);
    });
    ui->actionRunSimulation->setEnabled(project->getSystemSetup()->getStructureReady());

    auto setCoordsFile = [this] (const QString& fileName) {
      setMoleculeFile(fileName);
    };
    conns << connect(
      systemSetup,
      &Model::SystemSetup::sourceStructureFileChanged,
      setCoordsFile);

    conns << connect(
      systemSetup,
      &Model::SystemSetup::processedStructureFileChanged,
      setCoordsFile);

    QWebEngineSettings* settings = ui->molpreview->page()->settings();
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    conns << connect(
      project.get(),
      &Model::Project::stepRemoved,
      [this] (std::shared_ptr<Model::Simulation>, int at) {
        queue->remove(at);
        removeTabAt(at + 1);
      });


    conns << connect(project.get(), &Model::Project::nameChanged, [this, project] (const QString& newName) {
      bool canContinue = !newName.isEmpty();
      QString title = "GROMACS UI | " + newName;
      QString fileName = ProjectManager::getInstance()->getFileName();
      fileName.replace(QDir::homePath(), "~");
      if (!fileName.isEmpty())
      {
        title += " | " + fileName;
      }
      this->setWindowTitle(title);
      ui->actionAddStep->setEnabled(canContinue);
      ui->actionCreateDefaultSimulationSetup->setEnabled(canContinue);
      ui->actionRunSimulation->setEnabled(
        canContinue && !project->getSystemSetup()->getProcessedStructureFile().isEmpty());
    });

    ui->stepconfigurator->addTab(new SystemSetupForm(project), tr("System Setup"));
    auto* tabBar = ui->stepconfigurator->tabBar();
    tabBar->tabButton(0, QTabBar::RightSide)->deleteLater();
    tabBar->setTabButton(0, QTabBar::RightSide, 0);
    for (auto step: project->getSteps())
    {
      addTabForStep(step);
    }
  }
}

void MainWindow::addTabForStep(std::shared_ptr<Model::Simulation> simulation, int at)
{
  if (at == -1)
  {
    at = ui->stepconfigurator->count() - 1;
  }

  auto project = ProjectManager::getInstance()->getCurrentProject();
  auto command = std::make_shared<Command::RunSimulation>(project, simulation);
  connect(command.get(), &Command::RunSimulation::runningChanged,
          [this] (bool isRunning) {
            ui->actionRunSimulation->setEnabled(!isRunning);
          });
  queue->insert(at, command);

  // take system setup into account for tabs
  at += 1;

  connect(
    simulation.get(),
    &Model::Simulation::simulationTypeChanged,
    [this, simulation, at] (Model::Simulation::Type) {
      ui->stepconfigurator->setTabText(at, simulation->getName());
    });

  SimulationSetupForm* simulationForm = new SimulationSetupForm(project, simulation, command);
  connect(simulationForm, &SimulationSetupForm::displaySimulationData,
          this, &MainWindow::setMoleculeFile);
  ui->stepconfigurator->insertTab(at, simulationForm, simulation->getName());
}

void MainWindow::removeTabAt(int at)
{
  QWidget* widget = ui->stepconfigurator->widget(at);
  ui->stepconfigurator->removeTab(at);
  widget->deleteLater();
}

void MainWindow::setMoleculeFile(const QString& file, const QString& trajectory)
{
  QString viewerPath;
#ifdef MOLVIEWER
  viewerPath = QString(MOLVIEWER);
#endif
  if (!QFile(viewerPath).exists())
  {
    // override for local testing + if not yet installed
    viewerPath = QCoreApplication::applicationDirPath() + "/../share/viewer.html";
  }

  QUrl url = QUrl::fromLocalFile(viewerPath);
  if (!file.isEmpty())
  {
    url.setQuery(QUrlQuery({{ "structure", file }, { "trajectory", trajectory } }));
  }

  ui->molpreview->setUrl(url);
  ui->tabWidget->setCurrentIndex(0);
}
