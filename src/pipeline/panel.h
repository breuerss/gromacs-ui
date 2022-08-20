#ifndef PIPELINE_PANEL_H
#define PIPELINE_PANEL_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "connector.h"
#include "node.h"

namespace Pipeline {

class Panel : public QGraphicsScene
{

public:
  static Panel* getInstance();
  void reuseConnector(Connector* connector);
  void startConnector(Port* at);
  void stopConnector();
  void connectorAccepted();
  Connector* getActiveConnector();
  QGraphicsItem* startingNode = nullptr;

protected:
  void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
  Connector* activeConnector = nullptr;

private:
  Panel(QObject* parent = nullptr);
};

}

#endif
