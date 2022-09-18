#include "porttooltip.h"
#include "colors.h"
#include <QDebug>

namespace Pipeline { namespace View {

PortTooltip::PortTooltip(QGraphicsItem* parent)
  : RoundedRectItem(parent)
{
  setPen(QPen("transparent"));
  hide();
  setRadius(8);
  setPos(40, 40);
  setBrush(Colors::Black);
  setZValue(1000);

  categoryAndTypeText = new QGraphicsTextItem(this);
  categoryAndTypeText->setDefaultTextColor(Colors::White);
  auto font = categoryAndTypeText->font();
  font.setPixelSize(13);
  font.setWeight(QFont::Bold);
  categoryAndTypeText->setFont(font);
  categoryAndTypeText->setPos(indent, indent);

  fileNameText = new QGraphicsTextItem(this);
  fileNameText->setDefaultTextColor(Colors::White);
  font = fileNameText->font();
  font.setPixelSize(11);
  font.setWeight(QFont::Medium);
  fileNameText->setFont(font);
  fileNameText->setPos(
    categoryAndTypeText->mapToParent(categoryAndTypeText->boundingRect().bottomLeft())
    );
  update();
}

void PortTooltip::setCategory(Command::FileObject::Category newCategory)
{
  category = newCategory;
  update();
}

void PortTooltip::setFileName(const QString& newFileName)
{
  fileName = newFileName;
  update();
}

void PortTooltip::setFileTypes(const QList<Command::FileObject::Type>& newTypes)
{
  types = newTypes;
  update();
}

void PortTooltip::update()
{
  using FileObject = Command::FileObject;
  QStringList categoryAndTypeList;
  if (category != Category::Unknown)
  {
    categoryAndTypeList << FileObject::toString(category).toUpper();
  }
    
  QStringList typesString;
  for (auto type : types)
  {
    typesString << FileObject::toString(type).toUpper();
  }
  categoryAndTypeList << typesString.join(", ");

  QString categoryAndType = categoryAndTypeList.join(" | ");
  auto parent = this;
  categoryAndTypeText->setPlainText(categoryAndType);
  if (categoryAndType.isEmpty())
  {
    parent = nullptr;
  }
  categoryAndTypeText->setParentItem(parent);

  parent = this;
  fileNameText->setPlainText(fileName);
  if (fileName.isEmpty())
  {
    parent = nullptr;
    qDebug() << "no filename unset parent";
    qDebug() << childrenBoundingRect().size();
  }
  fileNameText->setParentItem(parent);
    qDebug() << childrenBoundingRect().size() << childItems().size();

  setSize(childrenBoundingRect().size() + QSize(2 * indent, 2 * indent));
}

} }
