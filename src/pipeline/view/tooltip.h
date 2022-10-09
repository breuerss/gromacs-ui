#ifndef PIPELINE_VIEW_TOOLTIP_H
#define PIPELINE_VIEW_TOOLTIP_H

#include "roundedrectitem.h"

namespace Pipeline { namespace View {

class Tooltip : public RoundedRectItem {

public:
  using RoundedRectItem::RoundedRectItem;
  Tooltip(QGraphicsItem* parent = nullptr);
  ~Tooltip();

  void setHeader(const QString& newHeader);
  void setText(const QString& newText);

  QVariant itemChange(GraphicsItemChange change, const QVariant &value);

protected:
  void update();

  const int indent = 5;
  QGraphicsTextItem* headerItem;
  QGraphicsTextItem* textItem;

  QString header;
  QString text;
};
} }

#endif
