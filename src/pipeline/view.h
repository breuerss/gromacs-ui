#ifndef PIPELINE_VIEW_H
#define PIPELINE_VIEW_H

#include <QGraphicsView>

namespace Pipeline {
class View: public QGraphicsView
{
public:
  View(QWidget* parent = nullptr);

protected slots:
  void wheelEvent(QWheelEvent *event);
  void keyPressEvent(QKeyEvent *event);
};
}

#endif
