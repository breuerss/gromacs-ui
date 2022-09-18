#include "roundedrectitem.h"

#include <QDebug>

namespace Pipeline { namespace View {

void RoundedRectItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  if (isSelected())
  {
    QPen pen;
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);
  }
  painter->setBrush(brush());
  painter->setPen(pen());
  painter->drawRoundedRect(rect(), xRadius, yRadius);
}

void RoundedRectItem::setRadius(const double radius)
{
  setRadiusX(radius);
  setRadiusY(radius);
}

void RoundedRectItem::setRoundedRect(const QRectF rect, const double radius_x, const double radius_y)
{
  setRect(rect);
  setRadiusX(radius_x);
  setRadiusY(radius_y);
  update();
}

QSizeF RoundedRectItem::getSize() const
{
  return rect().size();
}

void RoundedRectItem::setSize(const QSizeF& size)
{
  QRectF newRect = rect();
  newRect.setSize(size);
  setRect(newRect);
  update();
}

} }
