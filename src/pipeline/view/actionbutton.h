#ifndef PIPELINE_ACTIONBUTTON_H
#define PIPELINE_ACTIONBUTTON_H

#include <QPushButton>
#include <QGraphicsDropShadowEffect>

namespace Pipeline { namespace View {

class ActionButton : public QPushButton
{
public:
  ActionButton(
    QWidget* parent = nullptr);
  ActionButton(
    unsigned size = defaultSize,
    const QColor& color = "green",
    QWidget* parent = nullptr);
  void setColor(const QColor& hexColor);

protected:
  void enterEvent(QEvent* event);
  void leaveEvent(QEvent* event);

private:
  void updateStyle();
  static const unsigned defaultSize = 60;
  unsigned size;
  QColor color;
  QGraphicsDropShadowEffect* effect = nullptr;
};

} }

#endif
