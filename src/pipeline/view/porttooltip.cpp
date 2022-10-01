#include "porttooltip.h"
#include "colors.h"
#include <QDebug>

namespace Pipeline { namespace View {

PortTooltip::PortTooltip(QGraphicsItem* parent)
  : Tooltip(parent)
{
  setPen(QPen("transparent"));
  hide();
  setRadius(8);
  setPos(40, 40);
  setBrush(Colors::Black);
  setZValue(1000);

  fileNameLegend = new QGraphicsTextItem("", this);
  fileNameLegend->setDefaultTextColor(Colors::White);
  auto font = fileNameLegend->font();
  font.setPixelSize(11);
  font.setWeight(QFont::Medium);
  fileNameLegend->setFont(font);
  fileNameLegend->setPos(
    textItem->mapToParent(textItem->boundingRect().bottomLeft())
    );
  update();
}

QVariant PortTooltip::itemChange(GraphicsItemChange change, const QVariant& value)
{
  return Tooltip::itemChange(change, value);
}

PortTooltip::~PortTooltip()
{
  delete fileNameLegend;
}

void PortTooltip::setCategory(Command::InputOutput::Category newCategory)
{
  category = newCategory;
  update();
}

void PortTooltip::setFileName(const QString& newFileName)
{
  setText(newFileName);
  update();
}

void PortTooltip::setFileTypes(const QList<Command::FileObject::Type>& newTypes)
{
  types = newTypes;
  update();
}

void PortTooltip::setCanOpen(bool newCanOpen)
{
  canOpen = newCanOpen;
  update();
}

void PortTooltip::update()
{
  using InputOuput = Command::InputOutput;
  QStringList categoryAndTypeList;
  if (category != Category::Unknown)
  {
    categoryAndTypeList << InputOuput::toString(category).toUpper();
  }
    
  using FileObject = Command::FileObject;
  QStringList typesString;
  for (auto type : types)
  {
    typesString << FileObject::toString(type).toUpper();
  }
  categoryAndTypeList << typesString.join(", ");

  QString categoryAndType = categoryAndTypeList.join(" | ");
  setHeader(categoryAndType);


  auto parent = this;
  if (text.isEmpty())
  {
    parent = nullptr;
  }
  fileNameLegend->setParentItem(parent);
  QString legend = tr("(Not yet generated)");
  if (canOpen)
  {
    legend = tr("(Click to open)");
  }
  fileNameLegend->setPlainText(legend);

  Tooltip::update();
}

} }
