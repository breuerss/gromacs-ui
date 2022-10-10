#include "node.h"
#include "panel.h"
#include "colors.h"
#include "tooltipmanager.h"
#include "styling.h"
#include "../step.h"
#include "../../uiupdater.h"
#include "../../config/configuration.h"
#include "../../command/executor.h"
#include "../../command/fileobjectconsumer.h"
#include "../../command/fileobjectprovider.h"
#include "../../undoredo/helper.h"
#include <cmath>
#include <memory>
#include <QIcon>
#include <QBrush>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QPropertyAnimation>
#include <QApplication>
#include <QLayout>
#include <QDebug>
#include <QVBoxLayout>

namespace Pipeline { namespace View {

const double Node::PI = std::acos(-1);

Node::Node(std::shared_ptr<Pipeline::Step> newStep, QGraphicsItem* parent)
  : QGraphicsRectItem(parent)
  , text(new QGraphicsTextItem(newStep->getName(), this))
  , runIcon(new ClickableIcon(
      QIcon(":/icons/play.svg"),
      true,
      this))
  , step(newStep)
{
  setFlag(QGraphicsItem::ItemIsMovable);
  setFlag(QGraphicsItem::ItemSendsGeometryChanges);
  setAcceptHoverEvents(true);
  setHeightAndRadius();

  QPen pen(QColor(0, 0, 0, 0));
  setPen(pen);

  setupBackground();
  setupText();
  setupRunIcon();
  resize();
  setupPorts();
  tooltipBox = new Tooltip();
  delayedTooltip.setInterval(500);
  delayedTooltip.setSingleShot(true);
  conns << QObject::connect(&delayedTooltip, &QTimer::timeout, [this] () {
    if (!tooltipBox->scene())
    {
      scene()->addItem(tooltipBox);
    }

    TooltipManager::show(tooltipBox);
  });

  conns << QObject::connect(
    step.get(), &Pipeline::Step::locationChanged,
    [this] (const auto& location) {
      setPos(location);
      scene()->update();
    });

  auto config = step->getConfiguration();
  if (config)
  {
    auto ui = config->getUI();
    if (ui)
    {
      setupResizeAnimation();
      setupSettingsWidget();
    }
  }

  setColorForSelectionState();
}

Node::~Node()
{
  for (auto conn: conns)
  {
    QObject::disconnect(conn);
  }
  delete tooltipBox;
  delete resizeAnimation;
  delete proxySettingsWidget;
  delete text;
  delete runIcon;
  delete background;
}

void Node::setHeightAndRadius()
{
  const auto maxPortNumber = std::max(
    step->getFileObjectConsumer()->requires().size(),
    step->getFileObjectProvider()->provides().size()
  );
  const auto arcLength = maxPortNumber * (2 * Port::RADIUS + arcSpacing) + arcSpacing;

  radius = std::max(arcLength / PI, minHeight / 2);
  height = 2 * radius;
  portArea = (Port::RADIUS * 2 + arcSpacing) / radius;
}

void Node::setupBackground()
{
  background = new RoundedRectItem(0, 0, 0, height, this);
  background->setBrush(Colors::getColorFor(step->getCategory()));
  background->setRadiusX(height / 2);
  background->setRadiusY(height / 2);
  background->setPen(QPen(Colors::getColorFor(step->getCategory()).darker(135), 2));
  resize();
}

void Node::setupText()
{
  text->setPos(indent, (background->rect().height() - text->boundingRect().height()) / 2);
  text->setDefaultTextColor(Colors::White);
  auto font = text->font();
  font.setPixelSize(16);
  font.setWeight(QFont::Bold);
  text->setFont(font);
  text->setZValue(1);
}

void Node::setupRunIcon()
{
  if (!step->getCommand())
  {
    runIcon->setParentItem(nullptr);
    resize();
    return;
  }
  runIcon->setZValue(11);
  auto textDim = text->boundingRect();
  runIcon->setPos(
    text->x() + textDim.width() + spacing,
    rect().center().y() - runIcon->boundingRect().height() / 2);

  auto command = step->getCommand().get();
  auto enableRunIcon = [this, command] () {
    runIcon->setEnabled(command->canExecute());
  };
  conns << QObject::connect(command, &Command::Executor::canExecuteChanged, enableRunIcon);
  enableRunIcon();

  conns << QObject::connect(runIcon, &ClickableIcon::clicked, [this] () {
    TooltipManager::hide(tooltipBox);
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

  auto changeIcon = [this] (bool isRunning) {
    QString file = ":/icons/play.svg";
    if (isRunning)
    {
      file = ":/icons/pause.svg";
    }

    const auto icon = QIcon(file);
    runIcon->setIcon(icon, true);
  };

  conns << QObject::connect(
    command, &Command::Executor::runningChanged,
    changeIcon);
  changeIcon(command->isRunning());
}

void Node::resize()
{
  const static double minWidth = 120;
  double width = text->boundingRect().width();
  if (runIcon->parentItem())
  {
    width += runIcon->boundingRect().width();
  }
  width += spacing;
  width += 2 * indent;
  width = std::max(minWidth, width);
  background->setSize(QSize(width, height));
  setRect(background->rect());
}

void Node::setupPorts()
{
  using Category = Command::InputOutput::Category;
  using FileObject = Command::FileObject;
  for (const auto& data: step->getFileObjectProvider()->provides())
  {
    QColor color = Colors::getColorFor(Category::Configuration);
    if (Command::isSet<FileObject::Pointer>(data))
    {
      auto fileObject = std::get<FileObject::Pointer>(data);
      color = Colors::getColorFor(fileObject->type);
    }

    addOutputPort(data, color);
  }

  auto categories = step->getFileObjectConsumer()->requires().keys();
  for (const auto& category: categories)
  {
    addInputPort(category, Colors::getColorFor(category));
  }

  conns << QObject::connect(
    step->getFileObjectConsumer().get(),
    &Command::FileObjectConsumer::connectedToChanged,
    [this] (
      const Command::Data& data,
      Command::InputOutput::Category category,
      const auto& oldData
      ) {
      auto panel = dynamic_cast<Panel*>(scene());
      auto endPort = getInputPort(category);
      if (isSet(oldData))
      {
        auto startPort = panel->getOutputPort(oldData);
        panel->removeConnectorFor({ startPort, endPort });
      }

      if (isSet(data))
      {
        auto startPort = panel->getOutputPort(data);
        panel->addConnector({ startPort, endPort });
      }
    });
}

void Node::addInputPort(Command::InputOutput::Category category, const QColor& color)
{
  const auto& acceptedFileTypes = step->getFileObjectConsumer()->requires()[category];
  auto inputPort = createPort<InputPort>(color);
  inputPort->setAcceptedFileTypes(acceptedFileTypes);
  inputPort->setCategory(category);
  inputPorts << QPair<Command::InputOutput::Category, InputPort*>(category, inputPort);

  conns << QObject::connect(
    inputPort, &Port::connectedToChanged, 
    [this] (
      const Command::Data& newData,
      const Command::Data& oldData
      ) {
      UndoRedo::Stack::getInstance()->beginMacro("Change connection");
      if (Command::isSet(newData))
      {
        UndoRedo::Helper::connectTo(step->getFileObjectConsumer().get(), newData);
      }

      if (Command::isSet(oldData))
      {
        UndoRedo::Helper::disconnectFrom(step->getFileObjectConsumer().get(), oldData);
      }
      UndoRedo::Stack::getInstance()->endMacro();
    });
  arrangeInputPorts();
}

const Node::OutputPorts& Node::getOutputPorts() const
{
  return outputPorts;
}

void Node::setupSettingsWidget()
{
  auto ui = step->getConfiguration()->getUI();
  auto children = ui->findChildren<QWidget*>(QString(), Qt::FindChildrenRecursively);
  if (children.size() > 30)
  {
    return;
  }

  proxySettingsWidget = new QGraphicsProxyWidget(this);
  QPointF settingsPos = text->pos();
  settingsPos.ry() += text->boundingRect().size().height();
  auto transform = proxySettingsWidget->transform();
  proxySettingsWidget->setTransform(transform.translate(settingsPos.x(), settingsPos.y()));

  auto uiLayout = ui->layout();
  auto layouts = uiLayout->findChildren<QLayout*>(QString(), Qt::FindDirectChildrenOnly);
  auto layout = new QVBoxLayout;
  double minWidth = 0;
  for (auto child : layouts)
  {
    layout->addItem(uiLayout->takeAt(uiLayout->indexOf(child)));
    minWidth = std::max<double>(minWidth, child->sizeHint().width());
  }
  QSizeF minimumSize = text->boundingRect().size() + runIcon->boundingRect().size();

  auto widget = new QWidget();
  widget->setLayout(layout);
  widget->setStyleSheet(stylesheet);

  QSizeF newSize = background->getSize();
  if (minimumSize.width() > minWidth)
  {
    widget->setFixedWidth(minimumSize.width());
  }
  else
  {
    newSize.rwidth() = minWidth + 80;
  }

  proxySettingsWidget->setWidget(widget);
  proxySettingsWidget->setZValue(10);
  widget->hide();
  newSize.rheight() += widget->size().height();
  resizeAnimation->setEndValue(newSize);
}

void Node::setupResizeAnimation()
{
  resizeAnimation = new QPropertyAnimation(background, "size");
  resizeAnimation->setDuration(200);
  resizeAnimation->setStartValue(background->boundingRect().size());
  resizeAnimation->setEasingCurve(QEasingCurve::OutQuad);

  conns << QObject::connect(
    resizeAnimation, &QPropertyAnimation::stateChanged,
    [this] (QAbstractAnimation::State newState) {
    if (newState == QAbstractAnimation::Running)
    {
      proxySettingsWidget->hide();
    }
  });
  conns << QObject::connect(resizeAnimation, &QPropertyAnimation::finished, [this] () {
    if (resizeAnimation->direction() == QAbstractAnimation::Forward)
    {
      proxySettingsWidget->show();
    }
  });
  conns << QObject::connect(resizeAnimation, &QPropertyAnimation::valueChanged, [this] () {
    arrangeOutputPorts();
  });
}

void Node::addOutputPort(const Command::Data& data, const QColor& color)
{
  auto outputPort = createPort<OutputPort>(color);
  outputPort->setProvidedData(data);
  using namespace Command;
  outputPorts << QPair<Data, OutputPort*>(data, outputPort);

  if (Command::isSet<FileObject::Pointer>(data))
  {
    auto fileObject = std::get<FileObject::Pointer>(data);
    conns << QObject::connect(outputPort, &OutputPort::clicked, [this, fileObject] () {
      using FileObject = Command::FileObject;
      using Category = Command::InputOutput::Category;
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
        case Category::Text:
          UiUpdater::getInstance()->showTextFile(fileObject->getFileName());
          break;
        case Category::Graph:
          UiUpdater::getInstance()->showGraph(fileObject->getFileName());
          break;
        default:
          break;
      }
    });
  }
  arrangeOutputPorts();
}

QString Node::getCoordinatesPath()
{
  QString coordinatesPath;

  using namespace Command;
  if (coordinatesPath.isEmpty())
  {
    const auto& data = step->getFileObjectConsumer()
      ->getConnectedTo()[InputOutput::Category::Coordinates];
    if (isSet<FileObject::Pointer>(data))
    {
      const auto& fileObject = std::get<FileObject::Pointer>(data);
      if (fileObject->exists())
      {
        coordinatesPath = fileObject->getFileName();
      }
    }
  }

  return coordinatesPath;
}

void Node::arrangeOutputPorts()
{
  const double startAngle = (outputPorts.size() - 1) * portArea / 2.0;
  const double width = background->boundingRect().size().width();
  for (int index = 0; index < outputPorts.size(); index++)
  {
    const auto port = outputPorts[index].second;
    const auto angle = startAngle - (outputPorts.size() - 1 - index) * portArea;
    const QPointF circlePoint = getCirclePoint(radius, angle);
    port->setX((width - radius) + circlePoint.x() - Port::RADIUS);
    port->setY(radius           + circlePoint.y() - Port::RADIUS);
  }
}

void Node::arrangeInputPorts()
{
  const double startAngle = PI - (inputPorts.size() - 1) * portArea / 2.0;
  const double x = boundingRect().topLeft().x();
  for (int index = 0; index < inputPorts.size(); index++)
  {
    const auto port = inputPorts[index].second;
    const auto angle = startAngle + index * portArea;
    const QPointF circlePoint = getCirclePoint(radius, angle);
    port->setX(radius + x + circlePoint.x() - Port::RADIUS);
    port->setY(radius -     circlePoint.y() - Port::RADIUS);
  }
}

QPointF Node::getCirclePoint(double radius, double radians)
{
  return QPointF(
    radius * std::cos(radians),
    radius * std::sin(radians)
  );
}

InputPort* Node::getInputPort(Command::InputOutput::Category category)
{
  InputPort* port = nullptr;
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

QVariant Node::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemPositionHasChanged)
  {
    step->setLocation(scenePos());
  }

  return QGraphicsItem::itemChange(change, value);
}

void Node::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{
  tooltipBox->setHeader(step->getName());
  auto config = step->getConfiguration();
  if (config)
  {
    tooltipBox->setText(config->toString());
  }
  delayedTooltip.start();
}

void Node::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
  if (!tooltipBox->isVisible())
  {
    tooltipBox->setPos(event->scenePos() + QPointF(20, 20));
  }
}

void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
  tooltipBox->setParentItem(nullptr);
  delayedTooltip.stop();
  TooltipManager::hide(tooltipBox);
}

// necessary to detect release event
void Node::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mousePressEvent(event);
  startingPos = pos();
  setZValue(2);
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
      .length() > QApplication::startDragDistance()) {
    TooltipManager::hide(tooltipBox);
  }

  QGraphicsRectItem::mouseMoveEvent(event);
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

  auto modifiers = event->modifiers();
  if (!modifiers.testFlag(Qt::ControlModifier) && !modifiers.testFlag(Qt::ControlModifier))
  {
    if (!toggleSettings())
    {
      step->showUI(selected);
    }
    step->showStatusUI(selected);
  }

}

bool Node::toggleSettings()
{
  if (!proxySettingsWidget)
  {
    return false;
  }

  auto direction = QAbstractAnimation::Forward;
  int zValue = 2;
  if (proxySettingsWidget->isVisible())
  {
    zValue = 1;
    direction = QAbstractAnimation::Backward;
  }

  setZValue(zValue);
  resizeAnimation->setDirection(direction);
  resizeAnimation->start();

  return true;
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
    setColorForSelectionState();

    selectedChanged();
  }
}

void Node::setColorForSelectionState()
{
  float opacity = 0.75;
  if (selected)
  {
    opacity = 1;
  }
  setOpacity(opacity);
}

std::shared_ptr<Pipeline::Step> Node::getStep() const
{
  return step;
}

} }
