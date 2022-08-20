#ifndef PIPELINE_NODE_H
#define PIPELINE_NODE_H

#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>
#include <QObject>
#include <QList>
#include <QGraphicsEllipseItem>
#include "roundedrectitem.h"
#include "port.h"

namespace Pipeline {

class Node : public QGraphicsRectItem
{
public:
  Node(const QString& name, QGraphicsItem* parent = nullptr);

  Port* getInputPort(int at);
  Port* getOutputPort(int at);

  void addInputPort(const QColor& color);
  void addOutputPort(const QColor& color);

private:
  Port* createPort(const QColor& color, Port::Type type);

  void arrangeOutputPorts();
  void arrangeInputPorts();

  QPointF getCirclePointForAngle(double angle) const;

  void arrangePortsHeights(QList<Port*> ports);
  QGraphicsTextItem* text;
  RoundedRectItem* nodeBackground;
  QList<Port*> inputPorts;
  QList<Port*> outputPorts;
};
}

#endif
