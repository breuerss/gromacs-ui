#ifndef PIPELINE_VIEW_PORTTOOLTIP
#define PIPELINE_VIEW_PORTTOOLTIP

#include "roundedrectitem.h"
#include "../../command/fileobject.h"

namespace Pipeline { namespace View {

class PortTooltip : public RoundedRectItem {

public:
  using RoundedRectItem::RoundedRectItem;
  PortTooltip(QGraphicsItem* parent = nullptr);
  ~PortTooltip();

  void setCategory(Command::FileObject::Category category);
  void setFileName(const QString& newFileName);
  void setFileTypes(const QList<Command::FileObject::Type>& newTypes);
  void setCanOpen(bool newCanOpen);

private:
  void update();

  using Category = Command::FileObject::Category;
  Category category = Category::Unknown;
  QList<Command::FileObject::Type> types;
  QString fileName;

  const int indent = 5;
  QGraphicsTextItem* categoryAndTypeText;
  QGraphicsTextItem* fileNameText;
  QGraphicsTextItem* fileNameLegend;
  bool canOpen = false;
};
} }

#endif
