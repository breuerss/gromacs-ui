#include "port.h"
#include "connector.h"
#include "colors.h"
#include "porttooltip.h"
#include "panel.h"
#include <QGraphicsView>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QWidget>
#include <QObject>
#include <qabstractanimation.h>
#include <qpropertyanimation.h>

namespace Pipeline { namespace View {

Port::Port(
  double x, double y, Type type, QGraphicsItem* parent)
  : QGraphicsEllipseItem(x, y, 2 * RADIUS, 2 * RADIUS, parent)
  , type(type)
{
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
  setAcceptedMouseButtons(Qt::LeftButton);
  setAcceptDrops(type == Type::Input);
  setAcceptHoverEvents(true);

  setupTooltip();

  increaseSize = new QPropertyAnimation(this, "size");
  increaseSize->setStartValue(QSize(2 * RADIUS, 2 * RADIUS));
  increaseSize->setEndValue(QSize(2.4 * RADIUS, 2.4 * RADIUS));
  increaseSize->setDuration(200);
  increaseSize->setEasingCurve(QEasingCurve::OutQuad);
}

Port::~Port()
{
  QObject::disconnect(conn);
  deleted(this);
  delete increaseSize;
  delete tooltipBox;
}

void Port::setupTooltip()
{
  tooltipBox = new PortTooltip();
}

void Port::setSize(const QSizeF& newSize)
{
  auto currentDim = rect();
  auto oldWidth = currentDim.width();
  auto oldHeight = currentDim.height();
  currentDim.setWidth(newSize.width());
  currentDim.setHeight(newSize.height());

  double newX = currentDim.x() + ((oldWidth - currentDim.width()) / 2);
  currentDim.setX(newX);
  double newY = currentDim.y() + ((oldHeight - currentDim.height()) / 2);
  currentDim.setY(newY);

  setRect(currentDim);
}

QSizeF Port::getSize() const
{
  return rect().size();
}

void Port::setProvidedFileObject(std::shared_ptr<Command::FileObject> newFileObject)
{
  fileObject = newFileObject;

  QObject::disconnect(conn);

  setCategory(Command::FileObject::getCategoryFor(fileObject->type));
  tooltipBox->setFileTypes({ fileObject->type });
  auto setTooltipCallback = [this] (const QString& tooltip)
  {
    tooltipBox->setFileName(tooltip);
  };

  setTooltipCallback("");
  if (fileObject)
  {
    conn = QObject::connect(
      fileObject.get(), &Command::FileObject::fileNameChanged,
      setTooltipCallback
      );
    setTooltipCallback(fileObject->getFileName());
  }
}

void Port::setAcceptedFileTypes(const QList<Command::FileObject::Type>& newAcceptedFileTypes)
{
  acceptedFileTypes = newAcceptedFileTypes;
  tooltipBox->setFileTypes(acceptedFileTypes);
}

void Port::setCategory(Command::FileObject::Category category)
{
  tooltipBox->setCategory(category);
}

QVariant Port::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemScenePositionHasChanged || change == QGraphicsItem::ItemSceneHasChanged)
  {
    emit centerPositionChanged(getCenterInScene());
  }
  return QGraphicsItem::itemChange(change, value);
}

void Port::setConnected(bool newConnected)
{
  connected = newConnected;
}

QPointF Port::getCenterInScene() const
{
  return mapToScene(rect().center());
}

void Port::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{
  if (fileObject)
  {
    setCursor(Qt::PointingHandCursor);
  }

  if (!tooltipBox->scene())
  {
    scene()->addItem(tooltipBox);
  }
  tooltipBox->show();
  auto sceneScale = scene()->views()[0]->transform().m11();
  tooltipBox->setScale(1 / sceneScale);

  tooltipBox->setPos(mapToScene(rect().bottomRight()));

  auto viewer = scene()->views()[0];
  QRectF viewport = viewer->viewport()->rect();
  auto tooFarRight = viewer->mapToScene(viewport.toRect().bottomRight()) -
    tooltipBox->mapToScene(tooltipBox->rect().bottomRight());

  auto tooltipPos = tooltipBox->pos();
  if (tooFarRight.x() < 0)
  {
    tooltipPos.rx() += tooFarRight.x() - 20;
  }
  if (tooFarRight.y() < 0)
  {
    tooltipPos.ry() += tooFarRight.y() - 20;
  }
  tooltipBox->setPos(tooltipPos);


  if (fileObject && !fileObject->getFileName().isEmpty())
  {
    increaseSize->setDirection(QAbstractAnimation::Forward);
    increaseSize->start();
  }
}

void Port::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
  unsetCursor();
  tooltipBox->hide();
  if (fileObject && !fileObject->getFileName().isEmpty())
  {
    increaseSize->setDirection(QAbstractAnimation::Backward);
    increaseSize->start();
  }
}

void Port::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  setCursor(Qt::ClosedHandCursor);
  startingPos = event->scenePos();
}

void Port::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  dynamic_cast<Panel*>(scene())->stopConnector();
  unsetCursor();
  if (event->scenePos() == startingPos)
  {
    clicked();
  }
}

void Port::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
      .length() < QApplication::startDragDistance()) {
    return;
  }

  auto panel = dynamic_cast<Panel*>(scene());
  if (type == Type::Input)
  {
    if (connected)
    {
      Connector* connector = panel->getConnectorFor(this);
      panel->reuseConnector(connector);

      auto fileObject = connector->getStartingPort()->getFileObject();
      connectedToChanged(nullptr, fileObject);
    }
  }
  else
  {
    panel->startConnector(this);
  }

  QDrag* drag = new QDrag(this);
  QMimeData *mime = new QMimeData;
  drag->setMimeData(mime);
  drag->exec();

  unsetCursor();
  panel->stopConnector();
}

void Port::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
  auto panel = dynamic_cast<Panel*>(scene());
  if (!connected && parentItem() != panel->startingNode)
  {
    auto port = panel->getActiveConnector()->getStartingPort();
    if (acceptedFileTypes.contains(port->getFileObject()->type))
    {
      event->accept();
      return;
    }
  }

  event->ignore();
}

void Port::dropEvent(QGraphicsSceneDragDropEvent* /*event*/)
{
  auto panel = dynamic_cast<Panel*>(scene());
  auto connector = panel->getActiveConnector();
  connector->setEndingPort(this);
  auto startingPort = connector->getStartingPort();
  connectedToChanged(startingPort->getFileObject(), nullptr);
  panel->connectorAccepted();
}

} }
