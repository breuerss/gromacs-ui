#ifndef PIPELINE_CONNECTOR_H
#define PIPELINE_CONNECTOR_H

#include "inputport.h"
#include "outputport.h"
#include <QGraphicsPathItem>

namespace Pipeline { namespace View {

class Connector : public QGraphicsPathItem
{
public:
  Connector(OutputPort* startingPort);
  ~Connector();

  void setEndingPort(InputPort* endingPort);
  void redraw(const QPointF& endingPoint = QPointF());
  OutputPort* getStartingPort() const;
  InputPort* getEndingPort() const;

private:
  OutputPort* startingPort;
  InputPort* endingPort;
  QMetaObject::Connection startingPortConn;
  QMetaObject::Connection endingPortConn;
};

} }

#endif
