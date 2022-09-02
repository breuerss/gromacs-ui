#include "clickableicon.h"
#include "qcursor.h"
#include <QDebug>
#include <QIcon>
#include <QPainter>

namespace Pipeline { namespace View {

ClickableIcon::ClickableIcon(const QIcon& newIcon, QGraphicsItem* parent)
  : QObject(nullptr)
  , QGraphicsPixmapItem(newIcon.pixmap(35, 35), parent)
  , icon(newIcon)
{
  setAcceptHoverEvents(true);
}

void ClickableIcon::setIcon(const QIcon& newIcon)
{
  icon = newIcon;
  setPixmap(icon.pixmap(pixmap().size()));
}

void ClickableIcon::setEnabled(bool enabled)
{
  QGraphicsPixmapItem::setEnabled(enabled);

  if (enabled) {
    setPixmap(icon.pixmap(pixmap().size()));
  }
  else
  {
    QPixmap disabledPixmap(pixmap().size());
    disabledPixmap.fill(Qt::transparent);
    QPainter p(&disabledPixmap);
    p.setOpacity(0.3);
    p.drawPixmap(0, 0, icon.pixmap(pixmap().size()));
    setPixmap(disabledPixmap);
  }
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


