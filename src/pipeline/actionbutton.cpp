#include "actionbutton.h"
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

namespace Pipeline {

ActionButton::ActionButton(QWidget* parent)
  : QPushButton(parent)
{
  int size = 60;
  setFixedWidth(size);
  setFixedHeight(size);
  setStyleSheet(
    QString("border-radius: %1;"
            "background-color: green;"
            "color: white;"
            "font-size: %2px;"
            "line-height: 1;"
            "padding: 0px;"
            "padding-top: -10px;"
            )
            .arg(size / 2)
            .arg(size / 5. * 3.)
    );
  effect = new QGraphicsDropShadowEffect();
  effect->setBlurRadius(10);
  effect->setOffset(0, 0);
  setGraphicsEffect(effect);
}

void ActionButton::enterEvent(QEvent*)
{
  effect->setBlurRadius(2);
  setCursor(Qt::ArrowCursor);
}

void ActionButton::leaveEvent(QEvent*)
{
  effect->setBlurRadius(10);
  unsetCursor();
}

}
