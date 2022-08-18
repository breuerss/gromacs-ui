#ifndef PIPELINE_CONNECTOR_H
#define PIPELINE_CONNECTOR_H

#include <QGraphicsPathItem>
#include "port.h"
namespace Pipeline {

class Connector : public QGraphicsPathItem
{
public:
  Connector(Port* startingPort);

  void setEndingPort(Port* endingPort);
  void setEndingPoint(const QPointF& endingPoint);

private:
  Port* startingPort;
  Port* endingPort;
  void redraw();

};

}

#endif
