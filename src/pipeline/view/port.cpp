#include "port.h"
#include "connector.h"
#include "qgraphicsitem.h"
#include "panel.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QWidget>

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
}

void Port::setProvidedFileObject(std::shared_ptr<Command::FileObject> newFileObject)
{
  fileObject = newFileObject;
}

void Port::setAcceptedFileTypes(const QList<Command::FileObject::Type>& newAcceptedFileTypes)
{
  acceptedFileTypes = newAcceptedFileTypes;
}

QVariant Port::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemScenePositionHasChanged)
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
}

void Port::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
  unsetCursor();
}

void Port::mousePressEvent(QGraphicsSceneMouseEvent* /*event*/)
{
  setCursor(Qt::ClosedHandCursor);
}

void Port::mouseReleaseEvent(QGraphicsSceneMouseEvent* /*event*/)
{
  dynamic_cast<Panel*>(scene())->stopConnector();
  unsetCursor();
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
      QList<QGraphicsItem*> list = panel->items();
      for (QGraphicsItem* item: list)
      {
        Connector* connector = dynamic_cast<Connector*>(item);
        if (connector && item == connector && connector->getEndingPort() == this)
        {
          panel->reuseConnector(connector);
          connector->setEndingPort(nullptr);
          panel->startingNode = connector->getStartingPort()->parentItem();
          break;
        }
      }
    }
  }
  else
  {
    panel->startConnector(this);
    panel->startingNode = parentItem();
  }

  QDrag* drag = new QDrag(this);
  QMimeData *mime = new QMimeData;
  drag->setMimeData(mime);
  drag->exec();

  unsetCursor();
  panel->stopConnector();
  panel->startingNode = nullptr;
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
  connectedToChanged(startingPort->getFileObject());
  panel->connectorAccepted();
}

} }