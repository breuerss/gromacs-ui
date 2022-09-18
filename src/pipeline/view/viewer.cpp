#include "viewer.h"

#include "panel.h"
#include "node.h"
#include "actionbutton.h"
#include "addmenu.h"
#include "../../model/project.h"
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

  createAddButton();
}

void Viewer::createAddButton()
{
  auto layout = new QGridLayout(this);
  layout->setContentsMargins(20, 20, 20, 20);
  layout->setRowStretch(0, 1);
  layout->setColumnStretch(0, 1);
  addButton = new ActionButton(this);
  addButton->setText("+");
  layout->addWidget(addButton);
  layout->setAlignment(addButton, Qt::AlignBottom | Qt::AlignLeft);
  addMenu = new AddMenu(addButton);
  connect(addButton, &QPushButton::clicked, addMenu, &AddMenu::toggle);
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

  }

  QGraphicsView::keyPressEvent(event);
}

void Viewer::center()
{
  resetTransform();
  const auto& rect = scene()->itemsBoundingRect();
  fitInView(rect + QMargins(10, 10, 10, 10), Qt::KeepAspectRatio);
}

void Viewer::drawForeground(QPainter *painter, const QRectF &rect)
{
  QGraphicsView::drawForeground(painter, rect);
}

} }
