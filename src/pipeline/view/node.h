#ifndef PIPELINE_NODE_H
#define PIPELINE_NODE_H

#include "roundedrectitem.h"
#include "port.h"
#include "outputport.h"
#include "inputport.h"
#include "tooltip.h"
#include "../../command/types.h"
#include "clickableicon.h"

#include <QGraphicsRectItem>
#include <QList>
#include <QTimer>
#include <memory>

class QPropertyAnimation;
class QGraphicsProxyWidget;

namespace Pipeline {

class Step;

namespace View {

class Node : public QObject, public QGraphicsRectItem
{
  Q_OBJECT
public:
  typedef QList<QPair<Command::Data, OutputPort*>> OutputPorts;

  Node(std::shared_ptr<Pipeline::Step> step, QGraphicsItem* parent = nullptr);
  ~Node();

  InputPort* getInputPort(Command::InputOutput::Category);
  const OutputPorts& getOutputPorts() const;

  void addInputPort(Command::InputOutput::Category category, const QColor& color);
  void addOutputPort(const Command::Data& fileObject, const QColor& color);

  std::shared_ptr<Pipeline::Step> getStep() const;
  QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value) override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void hoverEnterEvent(QGraphicsSceneHoverEvent*) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override;
  void hoverMoveEvent(QGraphicsSceneHoverEvent*) override;

private:
  template<typename PortType>
  PortType* createPort(const QColor& color)
  {
    auto port = new PortType(0, 0, this);
    port->setBrush(QBrush(color));
    port->setZValue(100);
    return port;
  }

  QString getCoordinatesPath();
  void resize();

  void arrangeOutputPorts();
  void arrangeInputPorts();
  static QPointF getCirclePoint(double radius, double angle);

  void setHeightAndRadius();

  void setupBackground();
  void setupText();
  void setupRunIcon();
  void setupPorts();
  void setupSettingsWidget();
  void setupResizeAnimation();
  void setColorForSelectionState();

  bool toggleSettings();

  QGraphicsTextItem* text;
  ClickableIcon* runIcon;
  RoundedRectItem* background;

  QList<QPair<Command::InputOutput::Category, InputPort*>> inputPorts;
  OutputPorts outputPorts;

  std::shared_ptr<Pipeline::Step> step;
  QPointF startingPos;

  const double indent = 30;
  const double spacing = 10;
  const double arcSpacing = 6;
  const double minHeight = 60;
  double radius = minHeight / 2;
  double height = minHeight;
  double portArea = (Port::RADIUS * 2 + arcSpacing) / radius;
  static const double PI;

  QGraphicsProxyWidget* proxySettingsWidget = nullptr;
  QPropertyAnimation* resizeAnimation = nullptr;
  Tooltip* tooltipBox;
  QTimer delayedTooltip;
  QList<QMetaObject::Connection> conns;
};

} }

#endif
