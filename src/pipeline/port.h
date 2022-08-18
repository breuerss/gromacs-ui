#ifndef PIPELINE_PORT_H
#define PIPELINE_PORT_H

#include <QGraphicsEllipseItem>

namespace Pipeline {

class Port : public QObject, public QGraphicsEllipseItem
{
  Q_OBJECT
public:
  const static unsigned int RADIUS = 8;
  Port(double x, double y, QGraphicsItem* parent = nullptr);

  QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value);
  QPointF getCenterInScene() const;

signals:
  void centerPositionChanged(const QPointF center);

};
}

#endif
