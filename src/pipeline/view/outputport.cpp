#include "outputport.h"
#include "panel.h"
#include <QPropertyAnimation>
#include <QApplication>
#include <QCursor>

namespace Pipeline { namespace View {

OutputPort::OutputPort(
  double x, double y, QGraphicsItem* parent)
  : Port(x, y, parent)
{
  setTransformOriginPoint(rect().center());
  increaseSize = new QPropertyAnimation(this, "scale");
  increaseSize->setStartValue(1);
  increaseSize->setEndValue(1.4);
  increaseSize->setDuration(200);
  increaseSize->setEasingCurve(QEasingCurve::OutQuad);
}

OutputPort::~OutputPort()
{
  delete increaseSize;
}

void OutputPort::setScale(double newScale)
{
  const bool changed = newScale != scale();
  QGraphicsEllipseItem::setScale(newScale);
  if (changed)
  {
    scaleChanged();
  }
}

void OutputPort::setProvidedFileObject(std::shared_ptr<Command::FileObject> newFileObject)
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

void OutputPort::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  if (hasData())
  {
    setCursor(Qt::PointingHandCursor);
  }

  tooltipBox->setCanOpen(hasData());
  if (hasData())
  {
    increaseSize->setDirection(QAbstractAnimation::Forward);
    increaseSize->start();
  }

  Port::hoverEnterEvent(event);
}

void OutputPort::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  unsetCursor();
  if (hasData())
  {
    increaseSize->setDirection(QAbstractAnimation::Backward);
    increaseSize->start();
  }

  Port::hoverLeaveEvent(event);
}

bool OutputPort::hasData()
{
  return fileObject && fileObject->exists();
}

void OutputPort::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  Port::mouseReleaseEvent(event);
  if (event->scenePos() == startingPos && hasData())
  {
    clicked();
  }
}

void OutputPort::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
      .length() < QApplication::startDragDistance()) {
    return;
  }

  auto panel = dynamic_cast<Panel*>(scene());
  panel->startConnector(this);

  Port::mouseMoveEvent(event);
}

} }
