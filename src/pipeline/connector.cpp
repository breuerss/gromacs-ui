#include "connector.h"
#include "qcolor.h"
#include "qnamespace.h"
#include <QDebug>
#include <QPainter>
#include <QPainterPath>

namespace Pipeline {

Connector::Connector(Port* startingPort)
  : startingPort(startingPort)
  , endingPort(nullptr)
{
  QObject::connect(startingPort, &Port::centerPositionChanged, [this] () {
    redraw();
  });
  setFlag(QGraphicsItem::ItemStacksBehindParent);
}

void Connector::setEndingPort(Port* newEndingPort)
{
  if (endingPort)
  {
    QObject::disconnect(endingPort, &Port::centerPositionChanged, nullptr, nullptr);
  }
  endingPort = newEndingPort;
  QObject::connect(endingPort, &Port::centerPositionChanged, [this] () {
    redraw();
  });
  redraw();
}

void Connector::redraw()
{
  auto startingPoint = startingPort->getCenterInScene();
  auto endingPoint = endingPort->getCenterInScene();
  auto midPoint = (startingPoint + endingPoint) / 2;

  QPainterPath painterPath;
  painterPath.moveTo(startingPoint);
  auto buffer = QPointF(60, 0);

  painterPath.cubicTo(startingPoint, startingPoint + buffer, midPoint);
  painterPath.cubicTo(midPoint, endingPoint - buffer, endingPoint);

  QPen pen(QColorConstants::Svg::darkslategray);
  pen.setWidth(3);
  setPen(pen);
  setPath(painterPath);
}

}
