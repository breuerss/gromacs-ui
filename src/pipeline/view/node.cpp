#include "node.h"
#include "panel.h"
#include "port.h"
#include "../step.h"
#include "src/command/fileobject.h"
#include "src/pipeline/view/clickableicon.h"
#include <QBrush>
#include <QDebug>
#include <cmath>
#include <QIcon>

namespace Pipeline { namespace View {

Node::Node(std::shared_ptr<Pipeline::Step> newStep, QGraphicsItem* parent)
  : QGraphicsRectItem(parent)
  , text(new QGraphicsTextItem(newStep->getName(), this))
//  , settingsIcon(new ClickableIcon(
//      QIcon::fromTheme("emblem-system"), this
      //QIcon::fromTheme("system-run").pixmap(40, 40), this
//      ))
  , step(newStep)
{
  setFlag(QGraphicsItem::ItemIsMovable);
  QPen pen(QColor(0, 0, 0, 0));
  setPen(pen);

  setFlags(QGraphicsItem::ItemIsMovable);

  const double indent = 30;
  const double spacing = 10;
  const double height = 60;
  double width = std::max<double>(120, text->boundingRect().width() + 2 * indent);
  //width += settingsIcon->boundingRect().width();
  //width += spacing;

  nodeBackground = new RoundedRectItem(0, 0, width, height, this);
  setRect(nodeBackground->rect());
  nodeBackground->setBrush(QBrush(Qt::darkGray));
  nodeBackground->setRadiusX(height / 2);
  nodeBackground->setRadiusY(height / 2);
  text->setPos(indent, (nodeBackground->rect().height() - text->boundingRect().height()) / 2);
  text->setZValue(10);

  //settingsIcon->setZValue(11);
  //auto textDim = text->boundingRect();
  //settingsIcon->setPos(
  //  text->x() + textDim.width() + spacing,
  //  rect().center().y() - settingsIcon->boundingRect().height() / 2);
  //QObject::connect(settingsIcon, &ClickableIcon::clicked, [this] () {
  //  qDebug() << __PRETTY_FUNCTION__;
  //  step->showConfigUI();
  //});

  for (const auto& fileObject: step->getFileObjectProvider().provides())
  {
    addOutputPort(fileObject, getColorFor(fileObject->type));
  }

  auto inputPortConfigs = step->getFileObjectConsumer().requires();
  for (const auto& category: inputPortConfigs.keys())
  {
    addInputPort(inputPortConfigs[category], getColorFor(category));
  }
}

void Node::addInputPort(const QList<Command::FileObject::Type>& acceptedFileTypes, const QColor& color)
{
  auto inputPort = createPort(color, Port::Type::Input);
  inputPort->setAcceptedFileTypes(acceptedFileTypes);
  inputPorts << inputPort;
  arrangeInputPorts();
}

void Node::addOutputPort(std::shared_ptr<Command::FileObject> fileObject, const QColor& color)
{
  auto outputPort = createPort(color, Port::Type::Output);
  outputPort->setProvidedFileObject(fileObject);
  QObject::connect(
    outputPort, &Port::connectedToChanged, 
    [this] (std::shared_ptr<Command::FileObject> fileObject) {
      step->getFileObjectConsumer().connectTo(fileObject);
    });
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

QColor Node::getColorFor(Command::FileObject::Category category)
{
  using Category = Command::FileObject::Category;
  const static QMap<Category, QColor> colors = {
    { Category::Coordinates, 0xff5b81e4 },
    { Category::Trajectory, 0xffe7911d },
    { Category::Energy, 0xffc0c753 },
    { Category::Forces, 0xff9974aa },

    //0xffc1584f
  };

  return colors[category];
}

QColor Node::getColorFor(Command::FileObject::Type type)
{
  auto category = Command::FileObject::getCategoryFor(type);

  return getColorFor(category);
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

void Node::mousePressEvent(QGraphicsSceneMouseEvent*)
{
  // necessary to detect release event
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent*)
{
  selected = !selected;
  const auto currentColor = nodeBackground->brush().color();
  const unsigned factor = 150;
  auto newColor = currentColor.lighter(factor);
  if (selected)
  {
    newColor = currentColor.darker(factor);
  }

  nodeBackground->setBrush(newColor);
  // TODO deselect all
  step->showConfigUI(selected);
  step->showStatusUI(selected);
}

} }
