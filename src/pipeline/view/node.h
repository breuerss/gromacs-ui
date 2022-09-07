#ifndef PIPELINE_NODE_H
#define PIPELINE_NODE_H

#include <QGraphicsRectItem>
#include <QObject>
#include <QList>
#include <QGraphicsEllipseItem>
#include <memory>
#include "roundedrectitem.h"
#include "port.h"
#include "../../command/fileobject.h"
#include "clickableicon.h"

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

  void setPos(const QPointF& pos);

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
