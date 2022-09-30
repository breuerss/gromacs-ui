#ifndef PIPELINE_PORT_H
#define PIPELINE_PORT_H

#include "../../command/fileobject.h"
#include "porttooltip.h"
#include <QGraphicsEllipseItem>
#include <memory>
#include <qpropertyanimation.h>

namespace Pipeline { namespace View {

class Port : public QObject, public QGraphicsEllipseItem
{
  Q_OBJECT
public:
  const static unsigned int RADIUS = 8;
  enum class Type {
    Input,
    Output,
  };
  Port(double x, double y, Type type, QGraphicsItem* parent = nullptr);
  virtual ~Port();

  QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value) override;
  QPointF getCenterInScene() const;
  void setConnected(bool newConnected = true);

  void setCategory(Command::FileObject::Category category);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void hoverEnterEvent(QGraphicsSceneHoverEvent*) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override;
  PortTooltip* tooltipBox;
  QPointF startingPos;
  bool connected = false;

signals:
  void deleted(Port*);
  void centerPositionChanged(const QPointF& center);
  void connectedToChanged(
    std::shared_ptr<Command::FileObject> newFileObject,
    std::shared_ptr<Command::FileObject> oldFileObject
    );

private:
  void setupTooltip();

  Type type;
};

} }

#endif
