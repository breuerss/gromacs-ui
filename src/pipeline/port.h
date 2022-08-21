#ifndef PIPELINE_PORT_H
#define PIPELINE_PORT_H

#include <QGraphicsEllipseItem>

namespace Pipeline {

class Port : public QObject, public QGraphicsEllipseItem
{
  Q_OBJECT
public:
  const static unsigned int RADIUS = 8;
  enum class Type {
    Input,
    Output,
  };
  Port(double x, double y, Type type, QGraphicsItem* parent = nullptr);

  QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value) override;
  QPointF getCenterInScene() const;
  void setConnected(bool connected = true);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
  void dropEvent(QGraphicsSceneDragDropEvent *event) override;
  void hoverEnterEvent(QGraphicsSceneHoverEvent*) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override;

signals:
  void centerPositionChanged(const QPointF center);

private:
  Type type;
  bool connected = false;

};
}

#endif
