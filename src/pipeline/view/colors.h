#ifndef PIPELINE_VIEW_COLORS_H
#define PIPELINE_VIEW_COLORS_H

#include <QColor>
#include "../step.h"

namespace Pipeline { namespace View { namespace Colors {

const QColor Pink("#e43a82");
const QColor Green("#89c64d");
const QColor Violet("#9945ba");
const QColor Orange("#de6328");
const QColor Blue("#766be6");
const QColor Grey("#d3dad3");
const QColor DarkGrey("#333339");
const QColor Egg("#e0f4db");
const QColor Yellow("#f0b92d");
const QColor Black("#111111");
const QColor White("#eeeeee");

QColor getColorFor(Step::Category category);
QColor getColorFor(Command::InputOutput::Category category);
QColor getColorFor(Command::FileObject::Type type);

} } }

#endif
