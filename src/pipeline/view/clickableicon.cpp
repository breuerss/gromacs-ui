#include "clickableicon.h"
#include "qcursor.h"
#include <QDebug>
#include <QIcon>
#include <qnamespace.h>

namespace Pipeline { namespace View {

ClickableIcon::ClickableIcon(const QIcon& icon, QGraphicsItem* parent)
  : QObject(nullptr)
  , QGraphicsPixmapItem(icon.pixmap(35, 35), parent)
{
  setAcceptHoverEvents(true);
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
  setCursor(Qt::PointingHandCursor);
}

void ClickableIcon::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
  unsetCursor();
}

} }


