#include "view.h"

#include "panel.h"
#include "node.h"
#include "qnamespace.h"
#include <QWheelEvent>
#include <QMargins>
#include <QDebug>

namespace Pipeline {

View::View(QWidget* parent)
  : QGraphicsView(parent)
{
  setDragMode(QGraphicsView::ScrollHandDrag);
  setRenderHint(QPainter::Antialiasing);

  auto pipeline = Panel::getInstance();
  //setSceneRect(-100, -100, 1000, 1000);
  setScene(pipeline);
  auto node = new Node("Fetch PDB");
  centerOn(node);
  pipeline->addItem(node);
  auto node2 = new Node("Box");
  node->setPos(1, 1);
  pipeline->addItem(node2);
  node2->setPos(80, 80);
  auto connector = new Pipeline::Connector(node->getOutputPort(0));
  connector->setEndingPort(node2->getInputPort(0));
  pipeline->addItem(connector);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void View::wheelEvent(QWheelEvent *event)
{
  if(event->angleDelta().y() > 0)
    scale(1.25, 1.25);
  else
    scale(0.8, 0.8);
}

void View::keyPressEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_Home)
  {
    resetTransform();
    const auto& rect = scene()->itemsBoundingRect();
    fitInView(rect + QMargins(10, 10, 10, 10), Qt::KeepAspectRatio);
  }
}

}
