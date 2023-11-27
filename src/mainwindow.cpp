#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui/preferencesdialog.h"
#include "projectmanager.h"
#include "statusmessagesetter.h"
#include "logforwarder.h"
#include "uiupdater.h"
#include "pipeline/runner.h"
#include "io/gracereader.h"
#include "io/graphdata.h"
#include "undoredo/movecommand.h"
#include "undoredo/stack.h"
#include "model/project.h"
#include "pipeline/view/viewer.h"
#include "pipeline/view/panel.h"
#include "pipeline/view/node.h"
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
  auto appState = settings.value(Settings::APP_STATE).toByteArray();
  if (appState.isEmpty()) {
    tabifyDockWidget(ui->configurationDock, ui->textViewDock);
    tabifyDockWidget(ui->configurationDock, ui->logDock);
    tabifyDockWidget(ui->configurationDock, ui->graphDock);
    tabifyDockWidget(ui->configurationDock, ui->moleculeDock);
  } else {
    restoreState(appState);
  }
  auto menu = ui->menuView;
  menu->addAction(ui->configurationDock->toggleViewAction());
  menu->addAction(ui->logDock->toggleViewAction());
  menu->addAction(ui->statusDock->toggleViewAction());
  menu->addAction(ui->moleculeDock->toggleViewAction());
  menu->addAction(ui->textViewDock->toggleViewAction());
  menu->addAction(ui->graphDock->toggleViewAction());

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
      setTextFile("");
      setGraph("");
      setMoleculeFile("");
      ui->logOutput->setPlainText("");
      UiUpdater::getInstance()->showStatusUI(nullptr);
      UiUpdater::getInstance()->showConfigUI(nullptr);
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
  for (auto conn: conns)
  {
    disconnect(conn);
  }
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
  QString content;
  QString title = tr("File Content Viewer");
  QString toolTip;
  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    content = file.readAll();
    file.close();
    ui->textViewDock->raise();
    toolTip = fileName;
    title = QFileInfo(file).fileName();
  }

  ui->textView->setText(content);
  ui->textViewDock->setWindowTitle(title);
  ui->textViewDock->setToolTip(toolTip);
}

void MainWindow::setGraph(const QString& graphFile)
{
  auto graphData = IO::GraceReader::readFile(graphFile);

  auto chart = new QChart();
  if (graphData.data.size())
  {
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
    auto xAxis = chart->axes(Qt::Horizontal)[0];
    xAxis->setTitleText(graphData.xLabel);
    auto yAxis = chart->axes(Qt::Vertical)[0];
    yAxis->setTitleText(graphData.yLabel);
  }

  auto oldChart = graphView->chart();
  graphView->setChart(chart);
  if (oldChart)
  {
    delete oldChart;
  }

  QString title = tr("Graph Viewer");
  QString toolTip;
  if (!graphFile.isEmpty())
  {
    ui->graphDock->raise();
    toolTip = graphFile;
    title = QFileInfo(graphFile).fileName();
  }

  ui->graphDock->setToolTip(toolTip);
  ui->graphDock->setWindowTitle(title);
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

    auto updateTitle = [this, project] () {
      QStringList titleComps("GROMACS UI");

      const auto& project = ProjectManager::getInstance()->getCurrentProject();
      auto name = project->property("name").toString();

      titleComps << name;

      QString fileName = ProjectManager::getInstance()->getFileName();
      fileName.replace(QDir::homePath(), "~");
      if (!UndoRedo::Stack::getInstance()->isClean())
      {
        fileName += "*";
      }
      if (!fileName.isEmpty())
      {
        titleComps << fileName;
      }
      this->setWindowTitle(titleComps.join(" | "));
    };
    conns << connect(project.get(), &Model::Project::nameChanged, updateTitle);
    conns << connect(UndoRedo::Stack::getInstance(), &QUndoStack::cleanChanged, updateTitle);
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
  if (!file.isEmpty())
  {
    ui->moleculeDock->raise();
  }
}
