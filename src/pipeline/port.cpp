#include "port.h"
#include "qgraphicsitem.h"
#include <QDebug>

namespace Pipeline {

Port::Port(double x, double y, QGraphicsItem* parent)
  : QGraphicsEllipseItem(x - RADIUS, y - RADIUS, 2 * RADIUS, 2 * RADIUS, parent)
{
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
}

QVariant Port::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemScenePositionHasChanged)
  {
    emit centerPositionChanged(getCenterInScene());
  }
  return QGraphicsItem::itemChange(change, value);
}

QPointF Port::getCenterInScene() const
{
  return mapToScene(rect().center());
}

}


