#include "mainwindow.h"
#include "qgraphicsitem.h"
#include "src/gromacsconfigfilegenerator.h"
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
#include <memory>
#include "pipeline/panel.h"
#include "pipeline/node.h"
#include "pipeline/connector.h"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
  , preferencesDialog(new PreferencesDialog(this))
  , queue(std::make_shared<Command::Queue>())
{
  ui->setupUi(this);
  setGeometry(settings.value(Settings::APP_GEOMETRY, QRect(0, 0, 800, 600)).toRect());
  ui->splitter->setSizes({INT_MAX, INT_MAX});

  Pipeline::Panel* pipeline = Pipeline::Panel::getInstance();
  ui->pipelineView->setSceneRect(0, 0, ui->pipelineView->width(), ui->pipelineView->height());
  ui->pipelineView->setScene(pipeline);
  auto node = new Pipeline::Node("Fetch PDB");
  pipeline->addItem(node);
  auto node2 = new Pipeline::Node("Box");
  node->setPos(1, 1);
  pipeline->addItem(node2);
  node2->setPos(80, 80);
  auto connector = new Pipeline::Connector(node->getOutputPort(0));
  connector->setEndingPort(node2->getInputPort(0));
  pipeline->addItem(connector);

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

  connect(queue.get(), &Command::Queue::stepFinished,
          [this] (int stepIndex, std::shared_ptr<Command::Executor>, bool success) {
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
      auto manager = ProjectManager::getInstance();
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

      // import the existing definitions
      for (auto step: project->getSteps())
      {
        SimulationStatusChecker checker(project, step);
        if (checker.hasMdp())
        {
          GromacsConfigFileGenerator(step).setFromMdpFile(checker.getMdpPath());
        }
      }

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
    conns << connect(systemSetup, &Model::SystemSetup::configReadyChanged,
            ui->actionRunSimulation, &QAction::setEnabled);
    conns << connect(systemSetup, &Model::SystemSetup::configReadyChanged,
            ui->actionCreateDefaultSimulationSetup, &QAction::setEnabled);
    conns << connect(systemSetup, &Model::SystemSetup::configReadyChanged, [this] (bool ready) {
      int tabIndex = 0;
      if (!ready)
      {
        tabIndex = 2;
        ui->logOutput->setPlainText("");
      }
      ui->tabWidget->setCurrentIndex(tabIndex);
    });
    qDebug() << "structureReady"  << project->getSystemSetup()->configReady();
    project->getSystemSetup()->configReadyChanged(project->getSystemSetup()->configReady());

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
      QString title = "GROMACS UI | " + newName;
      QString fileName = ProjectManager::getInstance()->getFileName();
      fileName.replace(QDir::homePath(), "~");
      if (!fileName.isEmpty())
      {
        title += " | " + fileName;
      }
      this->setWindowTitle(title);
    });

    ui->stepconfigurator->addTab(new SystemSetupForm(project), tr("System Setup"));
    auto tabBar = ui->stepconfigurator->tabBar();
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
