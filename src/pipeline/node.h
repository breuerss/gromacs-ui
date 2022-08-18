#ifndef PIPELINE_NODE_H
#define PIPELINE_NODE_H

#include <QGraphicsItemGroup>
#include <QObject>
#include <QList>
#include <QGraphicsEllipseItem>
#include <qt5/QtWidgets/qgraphicsitem.h>
#include "roundedrectitem.h"
#include "port.h"

namespace Pipeline {

class Node : public QGraphicsItemGroup
{
public:
  Node(const QString& name);

  Port* getInputPort(int at);
  Port* getOutputPort(int at);

private:
  QGraphicsTextItem* text;
  RoundedRectItem* nodeBackground;
  QList<Port*> inputPorts;
  QList<Port*> outputPorts;
};
}

#endif
