#include "node.h"
#include "qcolor.h"
#include "qgraphicsitem.h"
#include "panel.h"
#include "port.h"
#include <QBrush>
#include <QDebug>
#include <cmath>
#include <qt5/QtWidgets/qgraphicsitem.h>
#include <ratio>

namespace Pipeline {

Node::Node(const QString& label, QGraphicsItem* parent)
  : QGraphicsRectItem(parent)
  , text(new QGraphicsTextItem(label, this))
{
  setFlag(QGraphicsItem::ItemIsMovable);
  QPen pen(QColor(0, 0, 0, 0));
  setPen(pen);

  //setHandlesChildEvents(false);

  const double indent = 30;
  const double height = 60;
  const double width = std::max<double>(120, text->boundingRect().width() + 2 * indent);
  nodeBackground = new RoundedRectItem(0, 0, width, height, this);
  setRect(nodeBackground->rect());
  nodeBackground->setBrush(QBrush(Qt::darkGray));
  nodeBackground->setRadiusX(height / 2);
  nodeBackground->setRadiusY(height / 2);
  text->setPos(indent, (nodeBackground->rect().height() - text->boundingRect().height()) / 2);
  text->setZValue(10);

  setFlags(QGraphicsItem::ItemIsMovable);

  addOutputPort(QColorConstants::Svg::darkorange);
  addOutputPort(QColorConstants::Svg::darkblue);
  addOutputPort(QColorConstants::Svg::darkred);

  addInputPort(QColorConstants::Svg::lightblue);
  addInputPort(QColorConstants::Svg::lightcyan);
}

void Node::addInputPort(const QColor& color)
{
  auto inputPort = createPort(color, Port::Type::Input);
  inputPorts << inputPort;
  arrangeInputPorts();
}

void Node::addOutputPort(const QColor& color)
{
  auto outputPort = createPort(color, Port::Type::Output);
  outputPorts << outputPort;
  arrangeOutputPorts();
}

Port* Node::createPort(const QColor& color, Port::Type type)
{
  auto port = new Port(0, 0, type, this);
  port->setBrush(QBrush(color));
  port->setZValue(100);
  return port;
}

void Node::arrangeOutputPorts()
{
  arrangePortsHeights(outputPorts);
  const double steps = 45;
  const double startAngle = 90 - steps * outputPorts.size() / 2.0 + steps / 2.0;
  const double radius = rect().height() / 2;
  const double width = rect().width();
  for (int index = 0; index < outputPorts.size(); index++)
  {
    auto port = outputPorts[index];
    const QPointF circlePount = getCirclePointForAngle(startAngle + index * steps);
    port->setX((width - radius) + circlePount.x() - Port::RADIUS);
    port->setY(radius           + circlePount.y() - Port::RADIUS);
  }
}

void Node::arrangeInputPorts()
{
  arrangePortsHeights(inputPorts);
  for (auto port: inputPorts)
  {
    port->setX(0. - Port::RADIUS);
  }
}

QPointF Node::getCirclePointForAngle(double angle) const
{
  const double pi = std::acos(-1);
  const double radians = angle * pi / 180;
  const double radius = rect().height() / 2;

  return QPointF(
    radius * std::sin(radians),
    radius * std::cos(radians)
  );
}

void Node::arrangePortsHeights(QList<Port*> ports)
{
  const double portDiameter = Port::RADIUS * 2;
  const double portDistance = 5;
  const int noOfPorts = ports.size();
  const double heightAllPortsWithSpace = noOfPorts * (portDiameter + portDistance) - portDistance;
  double start = (nodeBackground->rect().height() - heightAllPortsWithSpace) / 2.;
  for (int index = 0; index < ports.size(); index++)
  {
    ports[index]->setY(start + index * (portDiameter + portDistance));
  }
}

Port* Node::getInputPort(int at)
{
  return inputPorts[at];

}

Port* Node::getOutputPort(int at)
{
  return outputPorts[at];
}

}
