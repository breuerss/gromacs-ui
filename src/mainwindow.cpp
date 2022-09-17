#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "form/preferencesdialog.h"
#include "projectmanager.h"
#include "statusmessagesetter.h"
#include "logforwarder.h"
#include "uiupdater.h"

#include "model/systemsetup.h"
#include "model/project.h"

#include <QDebug>
#include <QWebEngineSettings>
#include <QUrlQuery>
#include <QDir>
#include <QToolButton>
#include <QCoreApplication>
#include <memory>
#include <QMenu>
#include <QBoxLayout>
#include "pipeline/view/viewer.h"
#include "pipeline/view/panel.h"
#include "pipeline/simulation/step.h"



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

  auto view = new Pipeline::View::Viewer(this);
  if (!ui->pipelineTab->layout())
  {
    auto layout = new QHBoxLayout;
    ui->pipelineTab->setLayout(layout);
  }
  ui->pipelineTab->layout()->addWidget(view);
  QWebEngineSettings* settings = ui->molpreview->page()->settings();
  settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
  settings->setAttribute(QWebEngineSettings::ShowScrollBars, false);
  setMoleculeFile();
  auto panel = new Pipeline::View::Panel(this);
  view->setScene(panel);
  panel->setProject(ProjectManager::getInstance()->getCurrentProject());

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

      using Config::Simulation;
      auto manager = ProjectManager::getInstance();
      auto project = manager->getCurrentProject();
      project->clearSteps();

      manager->currentProjectChanged(project);
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

void MainWindow::setupUIForProject()
{
  auto project = ProjectManager::getInstance()->getCurrentProject();

  return;
  if (project)
  {
    for (auto conn : conns)
    {
      disconnect(conn);
    }

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
