#include "addnodebutton.h"
#include <QDebug>

namespace Pipeline { namespace View {

AddNodeButton::AddNodeButton(QWidget* parent)
  : AddNodeButton("", defaultSize, defaultColor, parent)
{
}

AddNodeButton::AddNodeButton(
  const QString& text,
  unsigned size,
  const QColor& color,
  QWidget* parent)
  : ActionButton(size, color, parent)
{
  setText(text);
  setMinimumWidth(0);
  setMaximumWidth(16777215);

  updateStyle();
}

} }
