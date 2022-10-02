#include "actionbutton.h"
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

namespace Pipeline { namespace View {

QColor ActionButton::defaultColor = Colors::Green;

ActionButton::ActionButton(QWidget* parent)
  : ActionButton(defaultSize, defaultColor, parent)
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

ActionButton::~ActionButton()
{
  delete effect;
}

void ActionButton::setColor(const QColor& newColor)
{
  color = newColor;
  updateStyle();
}

void ActionButton::updateStyle()
{
  QString style = QString(
    "QPushButton {"
    "padding: 0px 20px;"
  );
  if (minimumWidth() == maximumWidth())
  {
    style += "padding: 0px;";
  }
  if (text() == "+")
  {
    style += QString(
      "padding-top: -10px;"
      );
  }

  style += QString(
    "border-radius: %1;"
    "background-color: " + color.name() + ";"
    "color: white;"
    "font-size: %2px;"
    "line-height: 1;"
    "}"
    "QLabel, QToolTip {"
    "background: black;"
    "padding: 5px;"
    "color: white;"
    "border-radius: 8px;"
    "}"
  )
    .arg(size / 2)
    .arg(size / 5. * 3.);

  setStyleSheet(style);
}

void ActionButton::setText(const QString& text)
{
  QPushButton::setText(text);
  updateStyle();
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
