#ifndef PIPELINE_VIEW_PORTTOOLTIP
#define PIPELINE_VIEW_PORTTOOLTIP

#include "tooltip.h"
#include "../../command/fileobject.h"

namespace Pipeline { namespace View {

class PortTooltip : public Tooltip {

public:
  PortTooltip(QGraphicsItem* parent = nullptr);
  ~PortTooltip();

  void setCategory(Command::InputOutput::Category category);
  void setFileName(const QString& newFileName);
  void setFileTypes(const QList<Command::FileObject::Type>& newTypes);
  void setCanOpen(bool newCanOpen);
  QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
  void update();

  using Category = Command::InputOutput::Category;
  Category category = Category::Unknown;
  QList<Command::FileObject::Type> types;

  QGraphicsTextItem* fileNameLegend;
  bool canOpen = false;
};
} }

#endif
