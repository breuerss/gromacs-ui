#include "port.h"
#include "connector.h"
#include "colors.h"
#include "porttooltip.h"
#include "panel.h"
#include <QGraphicsView>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QWidget>
#include <QObject>

namespace Pipeline { namespace View {

Port::Port(
  double x, double y, Type type, QGraphicsItem* parent)
  : QGraphicsEllipseItem(x, y, 2 * RADIUS, 2 * RADIUS, parent)
  , type(type)
{
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
  setAcceptedMouseButtons(Qt::LeftButton);
  setAcceptDrops(type == Type::Input);
  setAcceptHoverEvents(true);

  setupTooltip();
}

Port::~Port()
{
  deleted(this);
}

void Port::setupTooltip()
{
  tooltipBox = new PortTooltip();
}

void Port::setProvidedFileObject(std::shared_ptr<Command::FileObject> newFileObject)
{
  fileObject = newFileObject;

  QObject::disconnect(conn);

  setCategory(Command::FileObject::getCategoryFor(fileObject->type));
  tooltipBox->setFileTypes({ fileObject->type });
  auto setTooltipCallback = [this] (const QString& tooltip)
  {
    tooltipBox->setFileName(tooltip);
  };

  setTooltipCallback("");
  if (fileObject)
  {
    conn = QObject::connect(
      fileObject.get(), &Command::FileObject::fileNameChanged,
      setTooltipCallback
      );
    setTooltipCallback(fileObject->getFileName());
  }
}

void Port::setAcceptedFileTypes(const QList<Command::FileObject::Type>& newAcceptedFileTypes)
{
  acceptedFileTypes = newAcceptedFileTypes;
  tooltipBox->setFileTypes(acceptedFileTypes);
}

void Port::setCategory(Command::FileObject::Category category)
{
  tooltipBox->setCategory(category);
}

QVariant Port::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemScenePositionHasChanged || change == QGraphicsItem::ItemSceneHasChanged)
  {
    emit centerPositionChanged(getCenterInScene());
  }
  return QGraphicsItem::itemChange(change, value);
}

void Port::setConnected(bool newConnected)
{
  connected = newConnected;
}

QPointF Port::getCenterInScene() const
{
  return mapToScene(rect().center());
}

void Port::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  if (fileObject)
  {
    setCursor(Qt::PointingHandCursor);
  }

  scene()->addItem(tooltipBox);
  tooltipBox->show();
  tooltipBox->setPos(mapToScene(event->pos()));

  auto viewer = scene()->views()[0];
  QRectF viewport = viewer->viewport()->rect();
  auto tooFarRight = viewer->mapToScene(viewport.toRect().bottomRight()) -
    mapToScene(tooltipBox->boundingRect().bottomRight());
  if (tooFarRight.x() < 0)
  {
    auto tooltipPos = tooltipBox->pos();
    tooltipPos.rx() += tooFarRight.x() - 10;
    tooltipBox->setPos(tooltipPos);
  }
}

void Port::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
  unsetCursor();
  tooltipBox->hide();
}

void Port::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  setCursor(Qt::ClosedHandCursor);
  startingPos = event->scenePos();
}

void Port::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  dynamic_cast<Panel*>(scene())->stopConnector();
  unsetCursor();
  if (event->scenePos() == startingPos)
  {
    clicked();
  }
}

void Port::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
      .length() < QApplication::startDragDistance()) {
    return;
  }

  auto panel = dynamic_cast<Panel*>(scene());
  if (type == Type::Input)
  {
    if (connected)
    {
      Connector* connector = panel->getConnectorFor(this);
      auto fileObject = connector->getStartingPort()->getFileObject();
      connectedToChanged(nullptr, fileObject);
      panel->reuseConnector(connector);
    }
  }
  else
  {
    panel->startConnector(this);
    panel->startingNode = parentItem();
  }

  QDrag* drag = new QDrag(this);
  QMimeData *mime = new QMimeData;
  drag->setMimeData(mime);
  drag->exec();

  unsetCursor();
  panel->stopConnector();
  panel->startingNode = nullptr;
}

void Port::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
  auto panel = dynamic_cast<Panel*>(scene());
  if (!connected && parentItem() != panel->startingNode)
  {
    auto port = panel->getActiveConnector()->getStartingPort();
    if (acceptedFileTypes.contains(port->getFileObject()->type))
    {
      event->accept();
      return;
    }
  }

  event->ignore();
}

void Port::dropEvent(QGraphicsSceneDragDropEvent* /*event*/)
{
  auto panel = dynamic_cast<Panel*>(scene());
  auto connector = panel->getActiveConnector();
  connector->setEndingPort(this);
  auto startingPort = connector->getStartingPort();
  connectedToChanged(startingPort->getFileObject(), nullptr);
  panel->connectorAccepted();
}

} }
