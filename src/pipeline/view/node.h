#ifndef PIPELINE_NODE_H
#define PIPELINE_NODE_H

#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>
#include <QObject>
#include <QList>
#include <QGraphicsEllipseItem>
#include <memory>
#include "roundedrectitem.h"
#include "port.h"
#include "../../command/fileobject.h"

namespace Pipeline {

class Step;

namespace View {

class Node : public QGraphicsRectItem
{
public:
  Node(std::shared_ptr<Pipeline::Step> step, QGraphicsItem* parent = nullptr);

  Port* getInputPort(int at);
  Port* getOutputPort(int at);

  void addInputPort(const QColor& color);
  void addOutputPort(const QColor& color);

private:
  Port* createPort(const QColor& color, Port::Type type);

  void arrangeOutputPorts();
  void arrangeInputPorts();

  QPointF getCirclePointForAngle(double angle) const;
  QColor getColorFor(Command::FileObject::Category category);

  void arrangePortsHeights(QList<Port*> ports);
  QGraphicsTextItem* text;
  RoundedRectItem* nodeBackground;
  QList<Port*> inputPorts;
  QList<Port*> outputPorts;
  std::shared_ptr<Pipeline::Step> step;
};

} }

#endif
