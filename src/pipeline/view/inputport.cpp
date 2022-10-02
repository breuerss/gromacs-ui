#include "inputport.h"
#include "panel.h"
#include "connector.h"
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QDebug>

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

  if (connected)
  {
    auto panel = dynamic_cast<Panel*>(scene());
    panel->reuseConnectorFor(this);

    auto data = getData();
    connectedToChanged(Command::Data(), data);
  }


  Port::mouseMoveEvent(event);
}

Command::Data InputPort::getData()
{
  Command::Data data;
  if (connected)
  {
    auto panel = dynamic_cast<Panel*>(scene());
    Connector* connector = panel->getConnectorFor(this);
    data = connector->getStartingPort()->getProvidedData();
  }

  return data;
}

void InputPort::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
  auto panel = dynamic_cast<Panel*>(scene());
  if (parentItem() != panel->startingNode)
  {
    auto port = panel->getActiveConnector()->getStartingPort();
    const auto& data = port->getProvidedData();

    bool accepts = false;
    if (Command::isSet<Command::FileObject::Pointer>(data))
    {
      const auto& fileObject = std::get<Command::FileObject::Pointer>(data);
      accepts = acceptedFileTypes.contains(fileObject->type);
    }
    else if (std::holds_alternative<Config::Configuration::Pointer>(data))
    {
      accepts = (category == Command::InputOutput::Category::Configuration);
    }
    if (accepts)
    {
      event->accept();
      return;
    }
  }

  event->ignore();
}

void InputPort::dropEvent(QGraphicsSceneDragDropEvent* /*event*/)
{
  auto oldData = getData();

  auto panel = dynamic_cast<Panel*>(scene());
  if (connected)
  {
    panel->deleteConnectorFor(this);
  }

  auto connector = panel->getActiveConnector();
  connector->setEndingPort(this);
  auto startingInputPort = connector->getStartingPort();
  auto newData = startingInputPort->getProvidedData();
  connectedToChanged(newData, oldData);
  panel->connectorAccepted();
}

} }
