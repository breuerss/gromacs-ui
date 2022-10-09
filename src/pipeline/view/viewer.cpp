#include "viewer.h"
#include "node.h"
#include "actionbutton.h"
#include "addmenu.h"
#include "../../../ui/topmenu.h"
#include "../../model/project.h"
#include "../../undoredo/stack.h"
#include <QWheelEvent>
#include <QMargins>
#include <QDebug>
#include <QGraphicsWidget>
#include <QPushButton>
#include <QLayout>

namespace Pipeline { namespace View {

Viewer::Viewer(QWidget* parent)
  : QGraphicsView(parent)
{
  setDragMode(QGraphicsView::ScrollHandDrag);
  setRenderHint(QPainter::Antialiasing);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  auto layout = new QGridLayout(this);
  layout->setRowStretch(0, 1);
  layout->setRowStretch(1, 1);
  layout->setColumnStretch(0, 1);
  layout->setContentsMargins(QMargins());

  createTopMenu();
  createAddButton();
}

Viewer::~Viewer()
{
  delete topMenu;
  delete addMenu;
  delete addButton;
}

void Viewer::createTopMenu()
{
  auto gridLayout = dynamic_cast<QGridLayout*>(layout());
  topMenu = new TopMenu(this);
  gridLayout->addWidget(topMenu, 0, 0);
  gridLayout->setAlignment(topMenu, Qt::AlignTop);

  auto setAlignment = [this] (Panel::Alignment alignment) {
    auto panel = dynamic_cast<Pipeline::View::Panel*>(scene());
    panel->alignSelectedNodes(alignment);
  };
  connect(topMenu, &TopMenu::alignBottomClicked, [setAlignment] () {
    setAlignment(Panel::Alignment::Bottom);
  });
  connect(topMenu, &TopMenu::alignTopClicked, [setAlignment] () {
    setAlignment(Panel::Alignment::Top);
  });
  connect(topMenu, &TopMenu::alignHCenterClicked, [setAlignment] () {
    setAlignment(Panel::Alignment::Center);
  });
  connect(topMenu, &TopMenu::alignLeftClicked, [setAlignment] () {
    setAlignment(Panel::Alignment::Left);
  });
  connect(topMenu, &TopMenu::alignRightClicked, [setAlignment] () {
    setAlignment(Panel::Alignment::Right);
  });
  connect(topMenu, &TopMenu::alignVCenterClicked, [setAlignment] () {
    setAlignment(Panel::Alignment::Middle);
  });

  auto distribute = [this] (Panel::Distribution direction) {
    auto panel = dynamic_cast<Pipeline::View::Panel*>(scene());
    panel->distributeSelectedNodes(direction);
  };
  connect(topMenu, &TopMenu::distributeHorizontallyClicked, [distribute] () {
    distribute(Panel::Distribution::Horizontal);
  });
  connect(topMenu, &TopMenu::distributeVerticallyClicked, [distribute] () {
    distribute(Panel::Distribution::Vertical);
  });

  connect(topMenu, &TopMenu::deleteSelectedClicked, [this] () {
    auto panel = dynamic_cast<Pipeline::View::Panel*>(scene());
    panel->deleteSelectedNodes();
  });

}

void Viewer::createAddButton()
{
  auto gridLayout = dynamic_cast<QGridLayout*>(layout());
  auto containerLayout = new QHBoxLayout(this);
  containerLayout->setContentsMargins(20, 20, 20, 20);
  containerLayout->setAlignment(addButton, Qt::AlignBottom | Qt::AlignLeft);
  gridLayout->addItem(containerLayout, 1, 0);
  gridLayout->setAlignment(containerLayout, Qt::AlignBottom | Qt::AlignLeft);
  addButton = new ActionButton(this);
  addButton->setText("+");
  containerLayout->addWidget(addButton);
  addMenu = new AddMenu(addButton);
  connect(addButton, &QPushButton::clicked, addMenu, &AddMenu::toggle);
}

void Viewer::resizeEvent(QResizeEvent* event)
{
  QWidget::resizeEvent(event);
  center();
}

void Viewer::wheelEvent(QWheelEvent *event)
{
  const auto currentScaleFactor = transform().m11();

  float scaleByPercent = -20;
  if(event->angleDelta().y() > 0)
  {
    scaleByPercent = 25;
  }

  if (
    (scaleByPercent < 0 && currentScaleFactor < 0.3) ||
    (scaleByPercent >= 0 && currentScaleFactor > 5)
     )
  {
    scaleByPercent = 0;
  }

  if (event->modifiers().testFlag(Qt::ControlModifier) && scaleByPercent != 0)
  {
    scaleByPercent /= 10;
  }

  const float scaleFactor = 1 + (scaleByPercent / 100);
  scale(scaleFactor, scaleFactor);
}

void Viewer::keyPressEvent(QKeyEvent *event)
{
  using Alignment = Panel::Alignment;
  using Distribution = Panel::Distribution;
  auto panel = dynamic_cast<Pipeline::View::Panel*>(scene());

  double scale = 1;
  if (event->modifiers().testFlag(Qt::ShiftModifier))
  {
    scale = 10;
  }
  else if (event->modifiers().testFlag(Qt::AltModifier))
  {
    scale = 0.1;
  }
  switch(event->key())
  {
    case Qt::Key_Home:
      center();
      break;
    case Qt::Key_A:
      panel->setAllNodesSelected(panel->getSelectedNodes().size() == 0);
      break;
    case Qt::Key_X:
      panel->deleteSelectedNodes();
      break;
    case Qt::Key_L:
      panel->alignSelectedNodes(Alignment::Left);
      break;
    case Qt::Key_R:
      panel->alignSelectedNodes(Alignment::Right);
      break;
    case Qt::Key_C:
      panel->alignSelectedNodes(Alignment::Center);
      break;
    case Qt::Key_M:
      panel->alignSelectedNodes(Alignment::Middle);
      break;
    case Qt::Key_T:
      panel->alignSelectedNodes(Alignment::Top);
      break;
    case Qt::Key_B:
      panel->alignSelectedNodes(Alignment::Bottom);
      break;

    case Qt::Key_H:
      panel->distributeSelectedNodes(Distribution::Horizontal);
      break;
    case Qt::Key_V:
      panel->distributeSelectedNodes(Distribution::Vertical);
      break;

    case Qt::Key_Left:
      panel->moveSelectedNodesHorizontal(-10 * scale);
      break;
    case Qt::Key_Right:
      panel->moveSelectedNodesHorizontal(10 * scale);
      break;
    case Qt::Key_Up:
      panel->moveSelectedNodesVertical(-10 * scale);
      break;
    case Qt::Key_Down:
      panel->moveSelectedNodesVertical(10 * scale);
      break;

    case Qt::Key_Z:
      {
        auto modifiers = event->modifiers();
        if (modifiers.testFlag(Qt::ControlModifier))
        {
          if (modifiers.testFlag(Qt::ShiftModifier))
          {
            UndoRedo::Stack::getInstance()->redo();
          }
          else
          {
            UndoRedo::Stack::getInstance()->undo();
          }
        }
      }

  }

  QGraphicsView::keyPressEvent(event);
}

void Viewer::setScene(View::Panel* newScene)
{
  auto oldPanel = dynamic_cast<View::Panel*>(scene());
  if (oldPanel)
  {
    disconnect(oldPanel);
  }

  connect(
    newScene, &View::Panel::selectedNodesChanged,
    [this] (QList<Node*> selectedNodes) {
      topMenu->setAlignmentButtonsEnabled(selectedNodes.size() > 1);
      topMenu->setDistributionButtonsEnabled(selectedNodes.size() > 2);
      topMenu->setDeleteButtonEnabled(selectedNodes.size() > 0);
    });

  QGraphicsView::setScene(newScene);
}

void Viewer::center()
{
  resetTransform();
  auto rect = scene()->itemsBoundingRect();
  if (rect.isEmpty())
  {
    rect = viewport()->rect();
  }
  fitInView(rect + QMargins(10, 10, 10, 10), Qt::KeepAspectRatio);
}

void Viewer::drawForeground(QPainter *painter, const QRectF &rect)
{
  QGraphicsView::drawForeground(painter, rect);
}

} }
