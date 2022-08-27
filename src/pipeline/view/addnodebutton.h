#ifndef PIPELINE_ADDNODEBUTTON_H
#define PIPELINE_ADDNODEBUTTON_H

#include "actionbutton.h"

namespace Pipeline { namespace View {

class AddNodeButton : public ActionButton
{
public:
  AddNodeButton(
    QWidget* parent = nullptr);
  AddNodeButton(
    const QString& text,
    unsigned height = defaultSize,
    const QColor& color = "darkblue",
    QWidget* parent = nullptr);
};

} }

#endif
