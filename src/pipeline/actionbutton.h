#ifndef PIPELINE_ACTIONBUTTON_H
#define PIPELINE_ACTIONBUTTON_H

#include <QPushButton>
#include <QGraphicsDropShadowEffect>

namespace Pipeline {

class ActionButton : public QPushButton
{
public:
  ActionButton(QWidget* parent = nullptr);

protected:
  void enterEvent(QEvent* event);
  void leaveEvent(QEvent* event);

private:
  QGraphicsDropShadowEffect* effect = nullptr;
};

}

#endif
