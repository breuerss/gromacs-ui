#ifndef PIPELINE_PORT_H
#define PIPELINE_PORT_H

#include "../../command/types.h"
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
  Port(double x, double y, QGraphicsItem* parent = nullptr);
  virtual ~Port();

  QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value) override;
  QPointF getCenterInScene() const;

  void setCategory(Command::InputOutput::Category category);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void hoverEnterEvent(QGraphicsSceneHoverEvent*) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override;
  PortTooltip* tooltipBox;
  QPointF startingPos;
  Command::InputOutput::Category category;

signals:
  void centerPositionChanged(const QPointF& center);
  void connectedToChanged(
    const Command::Data& newData,
    const Command::Data& oldData
    );

private:
  void setupTooltip();
};

} }

#endif
