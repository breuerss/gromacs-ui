#include "clickableicon.h"
#include <QCursor>
#include <QDebug>
#include <QIcon>
#include <QPainter>

namespace Pipeline { namespace View {

ClickableIcon::ClickableIcon(
  const QIcon& newIcon,
  bool newGrayScale,
  QGraphicsItem* parent)
  : QObject(nullptr)
  , QGraphicsPixmapItem(newIcon.pixmap(35, 35), parent)
  , icon(newIcon)
  , grayScale(newGrayScale)
{
  setAcceptHoverEvents(true);
  repaint();
}

void ClickableIcon::setIcon(const QIcon& newIcon, bool newGrayScale)
{
  icon = newIcon;
  grayScale = newGrayScale;

  repaint();
}

void ClickableIcon::setEnabled(bool newEnabled)
{
  enabled = newEnabled;
  repaint();
}

void ClickableIcon::repaint()
{
  QPixmap pixMap = icon.pixmap(pixmap().size());
  if (grayScale)
  {
    pixMap = getGrayScale(pixMap);
  }

  if (enabled) {
    setPixmap(pixMap);
  }
  else
  {
    QPixmap grayScale(pixMap.size());
    grayScale.fill(Qt::transparent);
    QPainter p(&grayScale);
    p.setOpacity(0.5);
    p.drawPixmap(0, 0, pixMap);
    setPixmap(grayScale);
  }
}

QPixmap ClickableIcon::getGrayScale(const QPixmap& pixmap) const 
{
  QImage im = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
  for (int y = 0; y < im.height(); ++y) {
    QRgb *scanLine = (QRgb*)im.scanLine(y);
    for (int x = 0; x < im.width(); ++x) {
      QRgb pixel = *scanLine;
      uint ci = uint(qGray(pixel));
      *scanLine = qRgba(ci, ci, ci, qAlpha(pixel));
      ++scanLine;
    }
  }
  return QPixmap::fromImage(im);
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


