#include "port.h"
#include "connector.h"
#include "colors.h"
#include "panel.h"
#include "tooltipmanager.h"
#include <QGraphicsView>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QWidget>
#include <QObject>
#include <qabstractanimation.h>
#include <qpropertyanimation.h>

namespace Pipeline { namespace View {

Port::Port(
  double x, double y, Type type, QGraphicsItem* parent)
  : QGraphicsEllipseItem(x, y, 2 * RADIUS, 2 * RADIUS, parent)
  , type(type)
{
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
  setAcceptedMouseButtons(Qt::LeftButton);
  setAcceptDrops(type == Type::Input);
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

void Port::setCategory(Command::FileObject::Category category)
{
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
  tooltipBox->hide();
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
