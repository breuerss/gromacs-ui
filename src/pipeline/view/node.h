#ifndef PIPELINE_NODE_H
#define PIPELINE_NODE_H

#include "roundedrectitem.h"
#include "port.h"
#include "../../command/fileobject.h"
#include "clickableicon.h"

#include <QGraphicsRectItem>
#include <QList>
#include <memory>

namespace Pipeline {

class Step;

namespace View {

class Node : public QGraphicsRectItem
{
public:
  typedef QList<QPair<std::shared_ptr<Command::FileObject>, Port*>> OutputPorts;

  Node(std::shared_ptr<Pipeline::Step> step, QGraphicsItem* parent = nullptr);

  Port* getInputPort(int at);
  Port* getInputPort(Command::FileObject::Category);
  Port* getOutputPort(int at);
  const OutputPorts& getOutputPorts() const;

  void addInputPort(Command::FileObject::Category category, const QColor& color);
  void addOutputPort(std::shared_ptr<Command::FileObject> fileObject, const QColor& color);

  bool isSelected() const;
  void setSelected(bool newSelected);
  std::shared_ptr<Pipeline::Step> getStep() const;
  QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value) override;

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
  Port* createPort(
    const QColor& color,
    Port::Type type);

  void arrangeOutputPorts();
  void arrangeInputPorts();

  static QPointF getCirclePoint(double radius, double angle);

  QGraphicsTextItem* text;
  ClickableIcon* runIcon;
  RoundedRectItem* nodeBackground;

  QList<QPair<Command::FileObject::Category, Port*>> inputPorts;

  OutputPorts outputPorts;

  std::shared_ptr<Pipeline::Step> step;
  bool selected = false;
  QPointF startingPos;
};

} }

#endif
