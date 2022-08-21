#ifndef PIPELINE_VIEW_H
#define PIPELINE_VIEW_H

#include <QGraphicsView>
#include "actionbutton.h"

namespace Pipeline {
class View: public QGraphicsView
{
public:
  View(QWidget* parent = nullptr);

protected slots:
  void wheelEvent(QWheelEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void drawForeground(QPainter *painter, const QRectF &rect);

private:
  ActionButton* addButton;
};
}

#endif
