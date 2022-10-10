#include "clickableicon.h"
#include <QCursor>
#include <QDebug>
#include <QIcon>
#include <QPainter>

namespace Pipeline { namespace View {

const QSize ClickableIcon::defaultSize = QSize(42, 42);

ClickableIcon::ClickableIcon(
  const QIcon& newIcon,
  bool newGrayScale,
  QGraphicsItem* parent)
  : QObject(nullptr)
  , QGraphicsPixmapItem(newIcon.pixmap(defaultSize), parent)
  , icon(newIcon)
  , grayScale(newGrayScale)
{
  setAcceptHoverEvents(true);
  update();
}

QRectF ClickableIcon::boundingRect() const
{
  return QRectF(QPointF(0, 0), defaultSize);
}

void ClickableIcon::setIcon(const QIcon& newIcon, bool newGrayScale)
{
  icon = newIcon;
  grayScale = newGrayScale;

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
  if (grayScale)
  {
    pixMap = getGrayScale(pixMap);
  }

  QPixmap grayScale(pixMap.size());
  grayScale.fill(Qt::transparent);
  painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
  painter->setOpacity(enabled ? 0.6 : 0.20);
  painter->drawPixmap(0, 0, pixMap);
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


