#ifndef PIPELINE_VIEW_CLICKABLEICON_H
#define PIPELINE_VIEW_CLICKABLEICON_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QIcon>

namespace Pipeline { namespace View {

class ClickableIcon : public QObject, public QGraphicsPixmapItem
{
  Q_OBJECT
public:
  ClickableIcon(
    const QIcon& icon,
    bool grayScale = false,
    QGraphicsItem* parent = nullptr);
  void setIcon(const QIcon& icon, bool grayScale = false);
  void setEnabled(bool enabled);

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
  void repaint();
};

} }

#endif
