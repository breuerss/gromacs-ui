#include "clickableicon.h"
#include <QCursor>
#include <QDebug>
#include <QIcon>
#include <QPainter>

namespace Pipeline { namespace View {

const QSize ClickableIcon::defaultSize = QSize(40, 40);

ClickableIcon::ClickableIcon(
  const QIcon& newIcon,
  QGraphicsItem* parent)
  : QObject(nullptr)
  , QGraphicsItem(parent)
  , icon(newIcon)
{
  setAcceptHoverEvents(true);
  update();
}

QRectF ClickableIcon::boundingRect() const
{
  return QRectF(QPointF(0, 0), defaultSize);
}

void ClickableIcon::setIcon(const QIcon& newIcon)
{
  icon = newIcon;

  update();
}

void ClickableIcon::setEnabled(bool newEnabled)
{
  enabled = newEnabled;
  update();
}

void ClickableIcon::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  QPixmap pixMap = icon.pixmap(defaultSize);
  painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
  painter->setOpacity(enabled ? 0.7 : 0.20);
  painter->drawPixmap(0, 0, pixMap);
}

void ClickableIcon::mousePressEvent(QGraphicsSceneMouseEvent*)
{
  // necessary to react to mouseReleaseEvent
}

void ClickableIcon::mouseReleaseEvent(QGraphicsSceneMouseEvent*)
{
  clicked();
}
  
void ClickableIcon::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{
  if (isEnabled())
  {
    setCursor(Qt::PointingHandCursor);
  }
}

void ClickableIcon::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
  unsetCursor();
}


} }


