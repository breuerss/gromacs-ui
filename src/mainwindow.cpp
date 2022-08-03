#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "preferencesdialog.h"
#include "projectmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , preferencesDialog(new PreferencesDialog(this))
{
    ui->setupUi(this);
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::openPreferencesDialog);
    connect(ui->actionAddStep, &QAction::triggered, ProjectManager::getInstance(), &ProjectManager::addStep);

    setupUIForProject();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openPreferencesDialog()
{
    preferencesDialog->exec();
}

void MainWindow::setupUIForProject()
{
    auto project = ProjectManager::getInstance()->getCurrentProject();
    if (project)
    {
        for (const auto step: project->getSteps())
        {
            (void) step;
             //ui->stepconfigurator->addTab();
        }
    }
}
