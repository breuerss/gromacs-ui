#ifndef PIPELINE_ACTIONBUTTON_H
#define PIPELINE_ACTIONBUTTON_H

#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include "colors.h"

namespace Pipeline { namespace View {

class ActionButton : public QPushButton
{
public:
  ActionButton(
    QWidget* parent = nullptr);
  ActionButton(
    unsigned size = defaultSize,
    const QColor& color = defaultColor,
    QWidget* parent = nullptr);
  ~ActionButton();
  void setColor(const QColor& hexColor);
  void setText(const QString& text);

protected:
  void enterEvent(QEvent* event);
  void leaveEvent(QEvent* event);

protected:
  void updateStyle();
  static const unsigned defaultSize = 60;
  static QColor defaultColor;
  unsigned size;
  QColor color;

private:
  QGraphicsDropShadowEffect* effect = nullptr;
};

} }

#endif
