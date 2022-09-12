#include "node.h"
#include "panel.h"
#include "port.h"
#include "../step.h"
#include "../../uiupdater.h"
#include "src/command/executor.h"
#include "src/command/fileobject.h"
#include "src/command/fileobjectconsumer.h"
#include "src/command/fileobjectprovider.h"
#include "src/pipeline/view/clickableicon.h"
#include "src/pipeline/view/colors.h"
#include <QBrush>
#include <cmath>
#include <QIcon>
#include <memory>

namespace Pipeline { namespace View {

Node::Node(std::shared_ptr<Pipeline::Step> newStep, QGraphicsItem* parent)
  : QGraphicsRectItem(parent)
  , text(new QGraphicsTextItem(newStep->getName(), this))
  , runIcon(new ClickableIcon(
      QIcon::fromTheme("media-playback-start"),
      true,
      this))
  , step(newStep)
{
  setFlag(QGraphicsItem::ItemIsMovable);
  setFlag(QGraphicsItem::ItemSendsGeometryChanges);
  QPen pen(QColor(0, 0, 0, 0));
  setPen(pen);

  const double indent = 30;
  const double spacing = 10;
  const double height = 60;
  double width = std::max<double>(120, text->boundingRect().width() + 2 * indent);
  width += runIcon->boundingRect().width();
  width += spacing;

  nodeBackground = new RoundedRectItem(0, 0, width, height, this);
  setRect(nodeBackground->rect());
  nodeBackground->setBrush(Colors::getColorFor(step->category));
  nodeBackground->setRadiusX(height / 2);
  nodeBackground->setRadiusY(height / 2);
  nodeBackground->setPen(QPen(Colors::getColorFor(step->category).darker(135), 2));
  text->setPos(indent, (nodeBackground->rect().height() - text->boundingRect().height()) / 2);
  text->setDefaultTextColor("white");
  auto font = text->font();
  font.setPixelSize(16);
  font.setWeight(700);
  text->setFont(font);
  text->setZValue(1);

  runIcon->setZValue(11);
  auto textDim = text->boundingRect();
  runIcon->setPos(
    text->x() + textDim.width() + spacing,
    rect().center().y() - runIcon->boundingRect().height() / 2);

  auto command = step->getCommand().get();
  auto enableRunIcon = [this, command] () {
    runIcon->setEnabled(command->canExecute());
  };
  QObject::connect(command, &Command::Executor::canExecuteChanged, enableRunIcon);
  enableRunIcon();

  QObject::connect(runIcon, &ClickableIcon::clicked, [this] () {
    auto command = step->getCommand();
    if (command->isRunning())
    {
      command->stop();
    }
    else
    {
      command->exec();
    }
  });
  QObject::connect(
    command, &Command::Executor::runningChanged,
    [this] (bool isRunning) {
      auto icon = QIcon::fromTheme("media-playback-start");
      if (isRunning)
      {
        icon = QIcon::fromTheme("media-playback-pause");
      }

      runIcon->setIcon(icon, true);
  });


  for (const auto& fileObject: step->getFileObjectProvider()->provides())
  {
    addOutputPort(fileObject, Colors::getColorFor(fileObject->type));
  }

  auto categories = step->getFileObjectConsumer()->requires().keys();
  for (const auto& category: categories)
  {
    addInputPort(category, Colors::getColorFor(category));
  }

  QObject::connect(
    step->getFileObjectConsumer().get(),
    &Command::FileObjectConsumer::connectedToChanged,
    [this] (
      std::shared_ptr<Command::FileObject> fileObject,
      Command::FileObject::Category category) {
      if (fileObject)
      {
        auto panel = dynamic_cast<Panel*>(scene());
        auto startPort = panel->getPort(fileObject);
        auto endPort = getInputPort(category);
        panel->addConnector(startPort, endPort);
      }
      // TODO get port for category
      // get output port from connecting node
    });
}

void Node::addInputPort(Command::FileObject::Category category, const QColor& color)
{
  const auto& acceptedFileTypes = step->getFileObjectConsumer()->requires()[category];
  auto inputPort = createPort(color, Port::Type::Input);
  inputPort->setAcceptedFileTypes(acceptedFileTypes);
  inputPorts << QPair<Command::FileObject::Category, Port*>(category, inputPort);
  QObject::connect(
    inputPort, &Port::connectedToChanged, 
    [this] (
      std::shared_ptr<Command::FileObject> fileObject,
      std::shared_ptr<Command::FileObject> oldFileObject
      ) {
      if (fileObject)
      {
        step->getFileObjectConsumer()->connectTo(fileObject);
      }
      else
      {
        step->getFileObjectConsumer()->disconnectFrom(oldFileObject);
      }
    });
  arrangeInputPorts();
}

QVariant Node::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemPositionHasChanged)
  {
    step->location = QRectF(scenePos(), boundingRect().size());
  }

  return QGraphicsItem::itemChange(change, value);
}

const Node::OutputPorts& Node::getOutputPorts() const
{
  return outputPorts;
}

void Node::addOutputPort(std::shared_ptr<Command::FileObject> fileObject, const QColor& color)
{
  auto outputPort = createPort(color, Port::Type::Output);
  outputPort->setProvidedFileObject(fileObject);
  outputPorts << QPair<std::shared_ptr<Command::FileObject>, Port*>(
    fileObject, outputPort);
  QObject::connect(outputPort, &Port::clicked, [this, fileObject] () {
    using FileObject = Command::FileObject;
    using Category = FileObject::Category;
    switch (FileObject::getCategoryFor(fileObject->type))
    {
      case Category::Coordinates:
        UiUpdater::getInstance()->showCoordinates(fileObject->getFileName());
        break;
      case Category::Trajectory:
        UiUpdater::getInstance()->showTrajectory(
          getCoordinatesPath(),
          fileObject->getFileName());
        break;
      default:
        break;
    }
  });
  arrangeOutputPorts();
}

QString Node::getCoordinatesPath()
{
  QString coordinatesPath;

  using FileObject = Command::FileObject;
  for (auto port : outputPorts)
  {
    auto fileObject = port.first;
    if (FileObject::getCategoryFor(fileObject->type)
        == FileObject::Category::Coordinates &&
        fileObject->exists())
    {
      coordinatesPath = fileObject->getFileName();
    }
  }

  if (coordinatesPath.isEmpty())
  {
    auto fileObject = step->getFileObjectConsumer()
      ->getConnectedTo()[FileObject::Category::Coordinates];
    if (fileObject->exists())
    {
      coordinatesPath = fileObject->getFileName();
    }
  }

  return coordinatesPath;
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
  const double steps = 45;
  const double startAngle = (outputPorts.size() - 1) * steps / 2.0;
  const double radius = rect().height() / 2;
  const double width = rect().width();
  for (int index = 0; index < outputPorts.size(); index++)
  {
    const auto port = outputPorts[index];
    const auto angle = startAngle - (outputPorts.size() - 1 - index) * steps;
    const QPointF circlePoint = getCirclePoint(radius, angle);
    port.second->setX((width - radius) + circlePoint.x() - Port::RADIUS);
    port.second->setY(radius           + circlePoint.y() - Port::RADIUS);
  }
}

void Node::arrangeInputPorts()
{
  const double steps = 45;
  const double startAngle = 180 - (inputPorts.size() - 1) * steps / 2.0;
  const double radius = rect().height() / 2;
  const double x = boundingRect().topLeft().x();
  for (int index = 0; index < inputPorts.size(); index++)
  {
    const auto port = inputPorts[index].second;
    const auto angle = startAngle + index * steps;
    const QPointF circlePoint = getCirclePoint(radius, angle);
    port->setX(radius + x + circlePoint.x() - Port::RADIUS);
    port->setY(radius -     circlePoint.y() - Port::RADIUS);
  }
}

QPointF Node::getCirclePoint(double radius, double angle)
{
  static const double pi = std::acos(-1);
  const double radians = angle * pi / 180;

  return QPointF(
    radius * std::cos(radians),
    radius * std::sin(radians)
  );
}

Port* Node::getInputPort(int at)
{
  return inputPorts[at].second;
}

Port* Node::getInputPort(Command::FileObject::Category category)
{
  Port* port = nullptr;
  for (auto& inputPort : inputPorts)
  {
    if (inputPort.first == category)
    {
      port = inputPort.second;
      break;
    }
  }
  return port;
}

Port* Node::getOutputPort(int at)
{
  return outputPorts[at].second;
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  // necessary to detect release event
  QGraphicsRectItem::mousePressEvent(event);
  startingPos = pos();
  setZValue(2);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mouseReleaseEvent(event);
  setZValue(1);
  // Do not select if it is moved
  if (startingPos != pos())
  {
    setPos(pos());
    return;
  }

  setSelected(!selected);
  // TODO deselect all
  // TODO check if was dragged --> no selection state change
  if (step->getConfiguration())
  {
    step->getConfiguration()->showUI(selected);
  }
  step->showStatusUI(selected);
}

bool Node::isSelected() const
{
  return selected;
}

void Node::setSelected(bool newSelected)
{
  const bool changed = selected != newSelected;
  selected = newSelected;

  if (changed)
  {
    const unsigned factor = 125;
    const auto currentColor = nodeBackground->brush().color();
    auto newColor = currentColor.lighter(factor);
    if (selected)
    {
      newColor = currentColor.darker(factor);
    }
    nodeBackground->setBrush(newColor);
  }
}

std::shared_ptr<Pipeline::Step> Node::getStep() const
{
  return step;
}

} }
