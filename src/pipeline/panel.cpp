#include "panel.h"
#include "qgraphicssceneevent.h"
#include <memory>
#include <QDebug>

namespace Pipeline {

Panel* Panel::getInstance()
{
  static std::unique_ptr<Panel> instance;
  if (!instance)
  {
    instance.reset(new Panel);
    instance->setBackgroundBrush(QBrush("#aaaaaa"));
  }

  return instance.get();
}

void Panel::reuseConnector(Connector* connector)
{
  stopConnector();
  activeConnector = connector;
}

void Panel::startConnector(Port* at)
{
  stopConnector();
  activeConnector = new Connector(at);
  addItem(activeConnector);
}

void Panel::stopConnector()
{
  if (activeConnector)
  {
    removeItem(activeConnector);
    delete activeConnector;
    activeConnector = nullptr;
  }
}

void Panel::connectorAccepted()
{
  activeConnector = nullptr;
}

Panel::Panel(QObject* parent)
  : QGraphicsScene(parent)
{
}

void Panel::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
  if (activeConnector)
  {
    activeConnector->redraw(event->scenePos());
  }
  QGraphicsScene::dragMoveEvent(event);
}

Connector* Panel::getActiveConnector()
{
  return activeConnector;
}

}
