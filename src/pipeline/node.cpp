#include "node.h"
#include "qgraphicsitem.h"
#include <QBrush>
#include <QDebug>

namespace Pipeline {

Node::Node(const QString& label)
{
  text = new QGraphicsTextItem(label, this);

  double height = 60;
  double width = std::max<double>(120, text->boundingRect().width() + 60);
  nodeBackground = new RoundedRectItem(0, 0, width, height, this);
  nodeBackground->setBrush(QBrush(Qt::darkGray));
  nodeBackground->setRadiusX(height / 2);
  nodeBackground->setRadiusY(height / 2);
  text->setPos(30, (nodeBackground->rect().height() - text->boundingRect().height()) / 2);
  text->setZValue(10);

  setFlags(QGraphicsItem::ItemIsMovable);

  auto outputPort = new Port(
    width, height / 2, this);
  outputPort->setBrush(QBrush(QColorConstants::Svg::darkorange));
  outputPorts << outputPort;

  auto inputPort = new Port(
    0, height / 2, this);
  inputPort->setBrush(QBrush(QColorConstants::Svg::blue));
  inputPorts << inputPort;
  qDebug() << (QGraphicsItem*)inputPort << (QGraphicsItem*)outputPort;

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
