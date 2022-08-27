#include "actionbutton.h"
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

namespace Pipeline { namespace View {

ActionButton::ActionButton(QWidget* parent)
  : ActionButton(defaultSize, "green", parent)
{
}

ActionButton::ActionButton(unsigned size, const QColor& color, QWidget* parent)
  : QPushButton(parent)
  , size(size)
  , color(color)
{
  setFixedWidth(size);
  setFixedHeight(size);
  effect = new QGraphicsDropShadowEffect();
  effect->setBlurRadius(10);
  effect->setOffset(0, 0);
  setGraphicsEffect(effect);

  updateStyle();
}

void ActionButton::setColor(const QColor& newColor)
{
  color = newColor;
  updateStyle();
}

void ActionButton::updateStyle()
{
  setStyleSheet(
    QString("border-radius: %1;"
            "background-color: " + color.name() + ";"
            "color: white;"
            "font-size: %2px;"
            "line-height: 1;"
            "padding: 0px;"
            "padding-top: -10px;"
            )
            .arg(size / 2)
            .arg(size / 5. * 3.)
    );
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

} }
