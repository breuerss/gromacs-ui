#include "viewer.h"

#include "panel.h"
#include "node.h"
#include "actionbutton.h"
#include "../../model/project.h"
#include "src/pipeline/view/addmenu.h"
#include <QWheelEvent>
#include <QMargins>
#include <QDebug>
#include <QGraphicsWidget>
#include <QPushButton>
#include <QLayout>

namespace Pipeline { namespace View {

Viewer::Viewer(QWidget* parent)
  : QGraphicsView(parent)
{
  setDragMode(QGraphicsView::ScrollHandDrag);
  setRenderHint(QPainter::Antialiasing);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  createAddButton();
}

void Viewer::createAddButton()
{
  auto layout = new QGridLayout(this);
  layout->setContentsMargins(20, 20, 20, 20);
  layout->setRowStretch(0, 1);
  layout->setColumnStretch(0, 1);
  addButton = new ActionButton(this);
  addButton->setText("+");
  layout->addWidget(addButton);
  layout->setAlignment(addButton, Qt::AlignBottom | Qt::AlignLeft);
  addMenu = new AddMenu(addButton);
  connect(addButton, &QPushButton::clicked, addMenu, &AddMenu::toggle);
}

void Viewer::wheelEvent(QWheelEvent *event)
{
  if(event->angleDelta().y() > 0)
    scale(1.25, 1.25);
  else
    scale(0.8, 0.8);
}

void Viewer::keyPressEvent(QKeyEvent *event)
{
  switch(event->key())
  {
    case Qt::Key_Home:
      center();
      break;
    case Qt::Key_X:
      dynamic_cast<Pipeline::View::Panel*>(scene())->deleteSelectedNodes();
      break;
  }
}

void Viewer::center()
{
  resetTransform();
  const auto& rect = scene()->itemsBoundingRect();
  fitInView(rect + QMargins(10, 10, 10, 10), Qt::KeepAspectRatio);
}

void Viewer::drawForeground(QPainter *painter, const QRectF &rect)
{
  QGraphicsView::drawForeground(painter, rect);
}

} }
