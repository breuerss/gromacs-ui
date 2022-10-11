#ifndef PIPELINE_VIEW_CLICKABLEICON_H
#define PIPELINE_VIEW_CLICKABLEICON_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QIcon>

namespace Pipeline { namespace View {

class ClickableIcon : public QObject, public QGraphicsItem
{
  Q_OBJECT
public:
  ClickableIcon(
    const QIcon& icon,
    bool grayScale = false,
    QGraphicsItem* parent = nullptr);
  void setIcon(const QIcon& icon, bool grayScale = false);
  void setEnabled(bool enabled);
  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

signals:
  void clicked();
  
protected:
  QPixmap getGrayScale(const QPixmap& pixmap) const;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  void hoverEnterEvent(QGraphicsSceneHoverEvent*) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override;
  QIcon icon;
  bool grayScale = false;
  bool enabled = true;

private:
  static const QSize defaultSize;
};

} }

#endif
