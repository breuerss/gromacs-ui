#include "connector.h"
#include "qcolor.h"
#include "qnamespace.h"
#include <QDebug>
#include <QPainter>
#include <QPainterPath>

namespace Pipeline { namespace View {

Connector::Connector(Port* startingPort)
  : startingPort(startingPort)
  , endingPort(nullptr)
{
  startingPort->setConnected();
  conn = QObject::connect(startingPort, &Port::centerPositionChanged, [this] () {
    redraw();
  });
  setFlag(QGraphicsItem::ItemStacksBehindParent);
}

Connector::~Connector()
{
  QObject::disconnect(conn);
}

void Connector::setEndingPort(Port* newEndingPort)
{
  if (endingPort)
  {
    endingPort->setConnected(false);
    QObject::disconnect(endingPort, &Port::centerPositionChanged, nullptr, nullptr);
  }

  endingPort = newEndingPort;
  if (endingPort)
  {
    endingPort->setConnected();
    QObject::connect(endingPort, &Port::centerPositionChanged, [this] () {
      redraw();
    });
  }
  redraw();
}

void Connector::redraw(const QPointF& endingPointOverride)
{
  auto startingPoint = startingPort->getCenterInScene();
  auto endingPoint = endingPointOverride;
  if (endingPoint.isNull())
  {
    if (!endingPort)
    {
      return;
    }
    endingPoint = endingPort->getCenterInScene();
  }
  auto midPoint = (startingPoint + endingPoint) / 2;

  QPainterPath painterPath;
  painterPath.moveTo(startingPoint);
  auto buffer = QPointF(60, 0);

  painterPath.cubicTo(startingPoint, startingPoint + buffer, midPoint);
  painterPath.cubicTo(midPoint, endingPoint - buffer, endingPoint);

  QPen pen(QColorConstants::Svg::darkslategray);
  pen.setWidth(2);
  setPen(pen);
  setPath(painterPath);
}

Port* Connector::getStartingPort() const
{
  return startingPort;
}

Port* Connector::getEndingPort() const
{
  return endingPort;
}

} }