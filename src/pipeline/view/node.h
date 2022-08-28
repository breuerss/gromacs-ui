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
  Node(std::shared_ptr<Pipeline::Step> step, QGraphicsItem* parent = nullptr);

  Port* getInputPort(int at);
  Port* getOutputPort(int at);

  void addInputPort(const QList<Command::FileObject::Type>& acceptedFileTypes, const QColor& color);
  void addOutputPort(std::shared_ptr<Command::FileObject> fileObject, const QColor& color);

  std::shared_ptr<Pipeline::Step> getStep() const { return step ; }

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
  Port* createPort(
    const QColor& color,
    Port::Type type);

  void arrangeOutputPorts();
  void arrangeInputPorts();

  QPointF getCirclePointForAngle(double angle) const;
  QColor getColorFor(Command::FileObject::Category category);
  QColor getColorFor(Command::FileObject::Type type);

  void arrangePortsHeights(QList<Port*> ports);
  QGraphicsTextItem* text;
  ClickableIcon* settingsIcon;
  ClickableIcon* runIcon;
  RoundedRectItem* nodeBackground;
  QList<Port*> inputPorts;
  QList<Port*> outputPorts;

  std::shared_ptr<Pipeline::Step> step;
  bool selected = false;
};

} }

#endif
