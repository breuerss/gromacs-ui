#ifndef PIPELINE_VIEW_H
#define PIPELINE_VIEW_H

#include <QGraphicsView>

namespace Pipeline { namespace View {

class ActionButton;
class AddMenu;

class Viewer: public QGraphicsView
{
public:
  Viewer(QWidget* parent = nullptr);
  void center();

protected slots:
  void wheelEvent(QWheelEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void drawForeground(QPainter *painter, const QRectF &rect);

private:
  void createAddButton();
  ActionButton* addButton;
  AddMenu* addMenu;

};

} }

#endif
