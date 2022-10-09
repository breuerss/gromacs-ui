#include "connector.h"
#include "colors.h"
#include <QDebug>
#include <QPainter>
#include <QPainterPath>

namespace Pipeline { namespace View {

Connector::Connector(OutputPort* startingPort)
  : startingPort(startingPort)
  , endingPort(nullptr)
{
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

void Connector::setEndingPort(InputPort* newEndingPort)
{
  if (endingPort)
  {
    QObject::disconnect(endingPortConn);
  }

  endingPort = newEndingPort;
  if (endingPort)
  {
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
  auto startingPoint = startingPort->getCenterInScene() + QPointF(Port::RADIUS, 0);
  auto endingPoint = endingPointOverride;
  if (endingPoint.isNull())
  {
    if (!endingPort)
    {
      return;
    }
    endingPoint = endingPort->getCenterInScene() - QPointF(Port::RADIUS, 0);
  }
  auto midPoint = (startingPoint + endingPoint) / 2;

  QPainterPath painterPath;
  painterPath.moveTo(startingPoint);

  auto diff = endingPoint - startingPoint;
  auto buffer = QPointF(60, 0);
  if (diff.x() < 0)
  {
    buffer = QPointF(std::max<double>(60, std::abs(diff.x())), diff.y() / 4);
  }
  painterPath.cubicTo(startingPoint, startingPoint + buffer, midPoint);
  painterPath.cubicTo(midPoint, endingPoint - buffer, endingPoint);

  QPen pen(Colors::Grey);
  pen.setWidth(2);
  setPen(pen);
  setPath(painterPath);
}

OutputPort* Connector::getStartingPort() const
{
  return startingPort;
}

InputPort* Connector::getEndingPort() const
{
  return endingPort;
}

} }
