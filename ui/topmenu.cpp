#include "topmenu.h"
#include "ui_topmenu.h"
#include "connectionhelper.h"
#include "../src/projectmanager.h"
#include "../src/model/project.h"
#include "../src/undoredo/stack.h"
#include "../src/pipeline/runner.h"
#include <QPushButton>
#include <QResizeEvent>

TopMenu::TopMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TopMenu)
{
  ui->setupUi(this);

  setStyleSheet("background: transparent;");
  const static QString buttonStylesheet(
    "* {"
    "background: rgba(179, 179, 179, 1.0);"
    "height: 36;"
    "width: 36;"
    "border-radius: 18;"
    "}"
    "*:hover {"
    "background: rgba(230, 230, 230, 1.0);"
    "}"
    "*:disabled {"
    "background: rgba(179, 179, 179, 0.5);"
    "}"
    );
  for (auto button : findChildren<QPushButton*>())
  {
    button->setStyleSheet(buttonStylesheet);
  }
  ui->frame->setStyleSheet("background: transparent;");
  ui->projectNameLabel->setStyleSheet(
    "color: white;"
    "padding: 0 10;"
    "height: 37;"
    "border-top-left-radius: 18;"
    "border-bottom-left-radius: 18;"
    "background-color: rgba(77, 77, 77, 1.0);"
    );
  ui->projectName->setStyleSheet(
    "QLineEdit {"
    "background-color: rgba(179, 179, 179, 1.0);"
    "height: 37;"
    "padding: 0 10;"
    "border-top-right-radius: 18;"
    "border-bottom-right-radius: 18;"
    "}"
    );
  ui->projectName->setContextMenuPolicy(Qt::PreventContextMenu);

  auto connectProjectName = [this] (auto newProject) {
    disconnect(projectConn);
    projectConn = connectToLineEdit(ui->projectName, newProject, "name");
  };
  connectProjectName(ProjectManager::getInstance()->getCurrentProject());
  conns << connect(ProjectManager::getInstance(), &ProjectManager::currentProjectChanged,
          connectProjectName);

  conns << connect(ui->alignBottom, &QPushButton::clicked, this, &TopMenu::alignBottomClicked);
  conns << connect(ui->alignTop, &QPushButton::clicked, this, &TopMenu::alignTopClicked);
  conns << connect(ui->alignVCenter, &QPushButton::clicked, this, &TopMenu::alignVCenterClicked);
  conns << connect(ui->alignLeft, &QPushButton::clicked, this, &TopMenu::alignLeftClicked);
  conns << connect(ui->alignRight, &QPushButton::clicked, this, &TopMenu::alignRightClicked);
  conns << connect(ui->alignHCenter, &QPushButton::clicked, this, &TopMenu::alignHCenterClicked);

  conns << connect(ui->deleteSelected, &QPushButton::clicked, this, &TopMenu::deleteSelectedClicked);
  setDeleteButtonEnabled(false);
  conns << connect(ui->runAllButton, &QPushButton::clicked, [] () {
    Pipeline::Runner::getInstance()->startPipeline();
  });

  conns << connect(ui->distributeHorizontally, &QPushButton::clicked, this, &TopMenu::distributeHorizontallyClicked);
  conns << connect(ui->distributeVertically, &QPushButton::clicked, this, &TopMenu::distributeVerticallyClicked);

  setAlignmentButtonsEnabled(false);
  setDistributionButtonsEnabled(false);

  conns << connect(UndoRedo::Stack::getInstance(), &QUndoStack::canUndoChanged, ui->undoButton, &QPushButton::setEnabled);
  conns << connect(UndoRedo::Stack::getInstance(), &QUndoStack::undoTextChanged, [this] (const auto& undoText) {
    ui->undoButton->setToolTip(tr("Undo") + ": " + undoText);
  });
  conns << connect(UndoRedo::Stack::getInstance(), &QUndoStack::canRedoChanged, ui->redoButton, &QPushButton::setEnabled);
  conns << connect(UndoRedo::Stack::getInstance(), &QUndoStack::redoTextChanged, [this] (const auto& redoText) {
    ui->redoButton->setToolTip(tr("Redo") + ": " + redoText);
  });
  ui->redoButton->setEnabled(UndoRedo::Stack::getInstance()->canRedo());
  ui->undoButton->setEnabled(UndoRedo::Stack::getInstance()->canUndo());
  conns << connect(ui->undoButton, &QPushButton::clicked, UndoRedo::Stack::getInstance(), &QUndoStack::undo);
  conns << connect(ui->redoButton, &QPushButton::clicked, UndoRedo::Stack::getInstance(), &QUndoStack::redo);
}

void TopMenu::setAlignmentButtonsEnabled(bool enabled)
{
  ui->alignBottom->setEnabled(enabled);
  ui->alignTop->setEnabled(enabled);
  ui->alignVCenter->setEnabled(enabled);
  ui->alignLeft->setEnabled(enabled);
  ui->alignRight->setEnabled(enabled);
  ui->alignHCenter->setEnabled(enabled);
}

void TopMenu::setDistributionButtonsEnabled(bool enabled)
{
  ui->distributeHorizontally->setEnabled(enabled);
  ui->distributeVertically->setEnabled(enabled);
}

void TopMenu::setDeleteButtonEnabled(bool enabled)
{
  ui->deleteSelected->setEnabled(enabled);
}

void TopMenu::resizeEvent(QResizeEvent* event)
{
  const auto size = event->size();
  const bool visible = size.width() > 812;
  ui->alignBottom->setVisible(visible);
  ui->alignTop->setVisible(visible);
  ui->alignLeft->setVisible(visible);
  ui->alignRight->setVisible(visible);

  const bool distributeVisible = size.width() > 648;
  ui->distributeVertically->setVisible(distributeVisible);
  ui->distributeHorizontally->setVisible(distributeVisible);

  const bool alignVisible = size.width() > 553;
  ui->alignVCenter->setVisible(alignVisible);
  ui->alignHCenter->setVisible(alignVisible);

  const bool undoRedoVisible = size.width() > 458;
  ui->undoButton->setVisible(undoRedoVisible);
  ui->redoButton->setVisible(undoRedoVisible);

  const bool deleteButtonVisible = size.width() > 400;
  ui->deleteSelected->setVisible(deleteButtonVisible);
}

TopMenu::~TopMenu()
{
  for (auto conn : conns)
  {
    disconnect(conn);
  }
  delete ui;
}
