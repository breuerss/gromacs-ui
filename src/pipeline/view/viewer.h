#ifndef PIPELINE_VIEW_H
#define PIPELINE_VIEW_H

#include <QGraphicsView>
#include "panel.h"

class TopMenu;

namespace Pipeline { namespace View {

class ActionButton;
class AddMenu;

class Viewer: public QGraphicsView
{
public:
  Viewer(QWidget* parent = nullptr);
  ~Viewer();
  void center();

  void setScene(View::Panel* scene);

protected:
  void resizeEvent(QResizeEvent* event);

protected slots:
  void wheelEvent(QWheelEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void drawForeground(QPainter *painter, const QRectF &rect);

private:
  void createAddButton();
  void createTopMenu();
  ActionButton* addButton;
  AddMenu* addMenu;
  TopMenu* topMenu;
};

} }

#endif
