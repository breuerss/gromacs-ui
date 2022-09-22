#ifndef PIPELINE_VIEW_TOOLTIP_H
#define PIPELINE_VIEW_TOOLTIP_H

#include "roundedrectitem.h"

namespace Pipeline { namespace View {

class Tooltip : public RoundedRectItem {

public:
  using RoundedRectItem::RoundedRectItem;
  Tooltip(QGraphicsItem* parent = nullptr);

  void setHeader(const QString& newHeader);
  void setText(const QString& newText);

private:
  void update();

  QString header;
  QString text;

  const int indent = 5;
  QGraphicsTextItem* headerItem;
  QGraphicsTextItem* textItem;
};
} }

#endif
