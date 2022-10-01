#include "port.h"
#include "panel.h"
#include "tooltipmanager.h"
#include <QDebug>
#include <QApplication>
#include <QDrag>
#include <QMimeData>

namespace Pipeline { namespace View {

Port::Port(
  double x, double y, QGraphicsItem* parent)
  : QGraphicsEllipseItem(x, y, 2 * RADIUS, 2 * RADIUS, parent)
{
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
  setAcceptedMouseButtons(Qt::LeftButton);
  setAcceptHoverEvents(true);
  setTransformOriginPoint(rect().center());

  setupTooltip();
}

Port::~Port()
{
  deleted(this);
  delete tooltipBox;
}

void Port::setConnected(bool newConnected)
{
  connected = newConnected;
}

void Port::setupTooltip()
{
  tooltipBox = new PortTooltip();
}

void Port::setCategory(Command::InputOutput::Category newCategory)
{
  category = newCategory;
  tooltipBox->setCategory(category);
}

QVariant Port::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemScenePositionHasChanged || change == QGraphicsItem::ItemSceneHasChanged)
  {
    emit centerPositionChanged(getCenterInScene());
  }
  return QGraphicsItem::itemChange(change, value);
}

QPointF Port::getCenterInScene() const
{
  return mapToScene(rect().center());
}

void Port::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{
  if (!tooltipBox->scene())
  {
    scene()->addItem(tooltipBox);
  }
  tooltipBox->setPos(mapToScene(rect().bottomRight()));
  TooltipManager::show(tooltipBox);
}

void Port::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
  unsetCursor();
  TooltipManager::hide(tooltipBox);
}

void Port::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  setCursor(Qt::ClosedHandCursor);
  startingPos = event->scenePos();
}

void Port::mouseReleaseEvent(QGraphicsSceneMouseEvent*)
{
  dynamic_cast<Panel*>(scene())->stopConnector();
  unsetCursor();
}

void Port::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
      .length() < QApplication::startDragDistance()) {
    return;
  }

  QDrag* drag = new QDrag(this);
  QMimeData *mime = new QMimeData;
  drag->setMimeData(mime);
  drag->exec();

  unsetCursor();

  auto panel = dynamic_cast<Panel*>(scene());
  panel->stopConnector();
}

} }
