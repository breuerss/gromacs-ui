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

  QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value);
  QPointF getCenterInScene() const;
  void setConnected(bool connected = true);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
  void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
  void dropEvent(QGraphicsSceneDragDropEvent *event);

signals:
  void centerPositionChanged(const QPointF center);

private:
  Type type;
  bool connected = false;

};
}

#endif
