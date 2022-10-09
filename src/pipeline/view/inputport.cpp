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

  auto panel = dynamic_cast<Panel*>(scene());
  Connector* connector = panel->getConnectorFor(this);
  if (connector)
  {
    auto data = getData();

    panel->reuseConnectorFor(this);
    connectedToChanged(Command::Data(), data);
  }


  Port::mouseMoveEvent(event);
}

Command::Data InputPort::getData()
{
  Command::Data data;
  auto panel = dynamic_cast<Panel*>(scene());
  Connector* connector = panel->getConnectorFor(this);
  if (connector)
  {
    data = connector->getStartingPort()->getProvidedData();
  }

  return data;
}

void InputPort::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
  const auto panel = dynamic_cast<Panel*>(scene());
  const auto activeConnector = panel->getActiveConnector();

  if (parentItem() != panel->startingNode && activeConnector)
  {
    const auto& port = activeConnector->getStartingPort();
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

void InputPort::dropEvent(QGraphicsSceneDragDropEvent*)
{
  const auto panel = dynamic_cast<Panel*>(scene());
  const auto connector = panel->getActiveConnector();
  if (connector)
  {
    const auto oldData = getData();

    const auto startingInputPort = connector->getStartingPort();
    const auto newData = startingInputPort->getProvidedData();
    panel->stopConnector();
    if (oldData != newData)
    {
      connectedToChanged(newData, oldData);
    }
  }
}

} }
