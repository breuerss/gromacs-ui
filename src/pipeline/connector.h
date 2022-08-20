#ifndef PIPELINE_CONNECTOR_H
#define PIPELINE_CONNECTOR_H

#include <QGraphicsPathItem>
#include "port.h"
namespace Pipeline {

class Connector : public QGraphicsPathItem
{
public:
  Connector(Port* startingPort);
  ~Connector();

  void setEndingPort(Port* endingPort);
  void redraw(const QPointF& endingPoint = QPointF());
  Port* getStartingPort() const;
  Port* getEndingPort() const;

private:
  Port* startingPort;
  Port* endingPort;
  QMetaObject::Connection conn;
};

}

#endif
