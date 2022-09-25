#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui/preferencesdialog.h"
#include "projectmanager.h"
#include "statusmessagesetter.h"
#include "logforwarder.h"
#include "uiupdater.h"
#include "pipeline/runner.h"
#include "io/gracereader.h"
#include "undoredo/movecommand.h"
#include "undoredo/stack.h"
#include "model/systemsetup.h"
#include "model/project.h"
#include "pipeline/view/viewer.h"
#include "pipeline/view/panel.h"
#include "pipeline/simulation/step.h"

#include <QDebug>
#include <QWebEngineSettings>
#include <QUrlQuery>
#include <QDir>
#include <QToolButton>
#include <QCoreApplication>
#include <memory>
#include <QMenu>
#include <QBoxLayout>
#include <QSplineSeries>

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
  , preferencesDialog(new PreferencesDialog(this))
{
  ui->setupUi(this);
  setGeometry(settings.value(Settings::APP_GEOMETRY, QRect(0, 0, 800, 600)).toRect());
  restoreState(settings.value(Settings::APP_STATE).toByteArray());
  auto menu = ui->menuView;
  menu->addAction(ui->configurationDock->toggleViewAction());
  menu->addAction(ui->logDock->toggleViewAction());
  menu->addAction(ui->statusDock->toggleViewAction());
  menu->addAction(ui->moleculeDock->toggleViewAction());
  menu->addAction(ui->textViewDock->toggleViewAction());

  auto view = new Pipeline::View::Viewer(this);
  if (!ui->pipelineTab->layout())
  {
    auto layout = new QHBoxLayout;
    ui->pipelineTab->setLayout(layout);
  }
  ui->pipelineTab->layout()->addWidget(view);

  graphView = new QChartView();
  if (!ui->graphDockContents->layout())
  {
    auto layout = new QHBoxLayout;
    ui->graphDockContents->setLayout(layout);
  }
  ui->graphDockContents->layout()->addWidget(graphView);
  graphView->setRenderHint(QPainter::Antialiasing);

  QWebEngineSettings* settings = ui->molpreview->page()->settings();
  settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
  settings->setAttribute(QWebEngineSettings::ShowScrollBars, false);
  setMoleculeFile();
  auto panel = new Pipeline::View::Panel(this);
  view->setScene(panel);
  panel->setProject(ProjectManager::getInstance()->getCurrentProject());

  connect(panel, &Pipeline::View::Panel::nodeMoved, [] (Pipeline::View::Node *node, const QPointF oldPosition) {
    UndoRedo::Stack::getInstance()->push(new UndoRedo::MoveCommand(node, oldPosition));
  });

  connect(UndoRedo::Stack::getInstance(), &QUndoStack::canUndoChanged, ui->actionUndo, &QAction::setEnabled);
  connect(UndoRedo::Stack::getInstance(), &QUndoStack::canRedoChanged, ui->actionRedo, &QAction::setEnabled);
  connect(ui->actionUndo, &QAction::triggered, UndoRedo::Stack::getInstance(), &QUndoStack::undo);
  connect(ui->actionRedo, &QAction::triggered, UndoRedo::Stack::getInstance(), &QUndoStack::redo);

  connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::openPreferencesDialog);
  connect(ui->actionNewProject, &QAction::triggered,
          ProjectManager::getInstance(), &ProjectManager::createNewProject);
  connect(ui->actionAddStep, &QAction::triggered, [] () {
    ProjectManager::getInstance()->getCurrentProject()->addStep<Pipeline::Simulation::Step>();
  });
  connect(ui->actionSave, &QAction::triggered, [] () {
    ProjectManager::getInstance()->save();
  });
  connect(ui->actionSaveProjectAs, &QAction::triggered,
          ProjectManager::getInstance(), QOverload<>::of(&ProjectManager::saveAs));
  connect(ui->actionOpenProject, &QAction::triggered, [] () {
    ProjectManager::getInstance()->open();
  });

  connect(LogForwarder::getInstance(), &LogForwarder::addMessage,
          ui->logOutput, &QPlainTextEdit::appendPlainText);

  auto addWidgetToWidget = [] (QWidget* child, QWidget* parent)
  {
    auto layout = parent->layout();
    if (!layout)
    {
      layout = new QVBoxLayout();
      parent->setLayout(layout);
    }

    QLayoutItem *oldChild;
    while ((oldChild = layout->takeAt(0)) != nullptr) {
      delete oldChild->widget();
      delete oldChild;
    }

    if (child != nullptr)
    {
      layout->addWidget(child);
    }
  };
  connect(
    UiUpdater::getInstance(), &UiUpdater::showConfigUI,
    [this, addWidgetToWidget] (QWidget* widget)
    {
      addWidgetToWidget(widget, ui->configBox);
      if (widget)
      {
        ui->configurationDock->raise();
      }
    });
  connect(
    UiUpdater::getInstance(), &UiUpdater::showStatusUI,
    [this, addWidgetToWidget] (QWidget* widget)
    {
      addWidgetToWidget(widget, ui->statusBox);
    });

  connect(
    UiUpdater::getInstance(), &UiUpdater::showTrajectory,
    [this] (const QString& coordinates, const QString& trajectory)
    {
      setMoleculeFile(coordinates, trajectory);
    });

  connect(
    UiUpdater::getInstance(), &UiUpdater::showGraph,
    [this] (const QString& graph)
    {
      setGraph(graph);
    });

  connect(
    UiUpdater::getInstance(), &UiUpdater::showTextFile,
    [this] (const QString& textFile)
    {
      setTextFile(textFile);
    });

  connect(
    UiUpdater::getInstance(), &UiUpdater::showCoordinates,
    [this] (const QString& coordinates)
    {
      setMoleculeFile(coordinates);
    });

  connect(StatusMessageSetter::getInstance(), &StatusMessageSetter::messageChanged,
          [this] (const QString& message) {
            ui->statusbar->showMessage(message);
          });

  setupUIForProject();
  connect(
    ProjectManager::getInstance(), &ProjectManager::currentProjectChanged,
    [this, view] () {
      view->center();
      setupUIForProject();
    });

  connect(
    ui->actionCreateDefaultSimulationSetup,
    &QAction::triggered,
    [] () {
      auto manager = ProjectManager::getInstance();
      auto project = manager->getCurrentProject();
      project->createDefaultSimulationSetup();

      manager->currentProjectChanged(project);
    });

  connect(
    ui->actionRunSimulation, &QAction::triggered,
    [] () {
      Pipeline::Runner::getInstance()->startPipeline();
    });

  QFont font;
  font.setFamily("Courier");
  font.setStyleHint(QFont::Monospace);
  font.setFixedPitch(true);
  font.setPointSize(12);
  ui->textView->setFont(font);
  QFontMetrics metrics(font);
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
  settings.setValue(Settings::APP_STATE, saveState());
  settings.setValue(Settings::APP_GEOMETRY, geometry());
  QMainWindow::closeEvent(event);
}

void MainWindow::setTextFile(const QString& fileName)
{
  QFile file(fileName);
  file.open(QFile::ReadOnly);
  ui->textView->setText(file.readAll());
  file.close();

  ui->textViewDock->setWindowTitle(fileName);
  ui->textViewDock->raise();
}

void MainWindow::setGraph(const QString& graphFile)
{
  auto graphData = IO::GraceReader::readFile(graphFile);

  auto chart = new QChart();
  auto xSeries = graphData.data[0].entries;
  for (int index = 1; index < graphData.data.size(); index++)
  {
    auto inputSeries = graphData.data[index].entries;
    auto series = new QSplineSeries();
    for (int valueIndex = 0; valueIndex < inputSeries.size(); valueIndex++)
    {
      series->append(xSeries[valueIndex], inputSeries[valueIndex]);
    }
    series->setName(graphData.data[index].legend);
    chart->addSeries(series);
  }
  chart->setTitle(graphData.title);
  chart->createDefaultAxes();

  auto oldChart = graphView->chart();
  graphView->setChart(chart);
  if (oldChart)
  {
    delete oldChart;
  }

  ui->graphDock->setWindowTitle(graphFile);
  ui->graphDock->raise();
}

void MainWindow::setupUIForProject()
{
  auto project = ProjectManager::getInstance()->getCurrentProject();
  if (project)
  {
    for (auto conn : conns)
    {
      disconnect(conn);
    }

    auto updateTitle = [this, project] (const QString& projectName = "") {
      QStringList titleComps("GROMACS UI");

      auto name = projectName;
      if (projectName.isEmpty())
      {
        auto project = ProjectManager::getInstance();
        name = project->property("name").toString();
      }
      titleComps << name;

      QString fileName = ProjectManager::getInstance()->getFileName();
      fileName.replace(QDir::homePath(), "~");
      if (!fileName.isEmpty())
      {
        titleComps << fileName;
      }
      this->setWindowTitle(titleComps.join(" | "));
    };
    conns << connect(project.get(), &Model::Project::nameChanged, updateTitle);
    updateTitle();
  }
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
  ui->moleculeDock->setVisible(true);
  ui->moleculeDock->raise();
}
