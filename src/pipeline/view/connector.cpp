#include "connector.h"
#include "colors.h"
#include <QDebug>
#include <QPainter>
#include <QPainterPath>

namespace Pipeline { namespace View {

Connector::Connector(Port* startingPort)
  : startingPort(startingPort)
  , endingPort(nullptr)
{
  startingPort->setConnected();
  startingPortConn = QObject::connect(startingPort, &Port::centerPositionChanged, [this] () {
    redraw();
  });
  setFlag(QGraphicsItem::ItemStacksBehindParent);
}

Connector::~Connector()
{
  QObject::disconnect(startingPortConn);
  QObject::disconnect(endingPortConn);
}

void Connector::setEndingPort(Port* newEndingPort)
{
  if (endingPort)
  {
    endingPort->setConnected(false);
    QObject::disconnect(endingPortConn);
  }

  endingPort = newEndingPort;
  if (endingPort)
  {
    endingPort->setConnected();
    endingPortConn = QObject::connect(
      endingPort, &Port::centerPositionChanged,
      [this] () {
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

  QPen pen(Colors::Grey);
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
