#include "inputport.h"
#include "panel.h"
#include "connector.h"
#include <QGraphicsSceneMouseEvent>
#include <QApplication>

namespace Pipeline { namespace View {

InputPort::InputPort(
  double x, double y, QGraphicsItem* parent)
  : Port(x, y, parent)
{
  setAcceptDrops(true);
}

void InputPort::setAcceptedFileTypes(const QList<Command::FileObject::Type>& newAcceptedFileTypes)
{
  acceptedFileTypes = newAcceptedFileTypes;
  tooltipBox->setFileTypes(acceptedFileTypes);
}

const QList<Command::FileObject::Type>& InputPort::getAcceptedFileTypes() const
{
  return acceptedFileTypes;
}

void InputPort::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
      .length() < QApplication::startDragDistance()) {
    return;
  }

  auto panel = dynamic_cast<Panel*>(scene());
  if (connected)
  {
    Connector* connector = panel->getConnectorFor(this);
    panel->reuseConnector(connector);

    auto fileObject = connector->getStartingPort()->getFileObject();
    connectedToChanged(nullptr, fileObject);
  }


  Port::mouseMoveEvent(event);
}

void InputPort::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
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

void InputPort::dropEvent(QGraphicsSceneDragDropEvent* /*event*/)
{
  auto panel = dynamic_cast<Panel*>(scene());
  auto connector = panel->getActiveConnector();
  connector->setEndingPort(this);
  auto startingInputPort = connector->getStartingPort();
  connectedToChanged(startingInputPort->getFileObject(), nullptr);
  panel->connectorAccepted();
}

} }
