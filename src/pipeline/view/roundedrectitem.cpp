#include "roundedrectitem.h"

#include <QDebug>

namespace Pipeline { namespace View {

void RoundedRectItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  if (this->isSelected())
  {
    QPen pen;
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);
  }
  painter->setBrush(brush());
  painter->drawRoundedRect(this->rect(), xRadius, yRadius);
}

void RoundedRectItem::setRoundedRect(const QRectF rect, const double radius_x, const double radius_y)
{
  this->setRect(rect);
  this->setRadiusX(radius_x);
  this->setRadiusY(radius_y);
}

} }
