#ifndef PIPELINE_VIEW_H
#define PIPELINE_VIEW_H

#include <QGraphicsView>
#include "actionbutton.h"

namespace Pipeline { namespace View {

class Viewer: public QGraphicsView
{
public:
  Viewer(QWidget* parent = nullptr);

protected slots:
  void wheelEvent(QWheelEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void drawForeground(QPainter *painter, const QRectF &rect);

private:
  ActionButton* addButton;
};

} }

#endif
