#include "tooltip.h"
#include "colors.h"
#include <QDebug>

namespace Pipeline { namespace View {

Tooltip::Tooltip(QGraphicsItem* parent)
  : RoundedRectItem(parent)
{
  setPen(QPen("transparent"));
  hide();
  setRadius(8);
  setPos(40, 40);
  setBrush(Colors::Black);
  setZValue(1000);

  headerItem = new QGraphicsTextItem(this);
  headerItem->setDefaultTextColor(Colors::White);
  auto font = headerItem->font();
  font.setPixelSize(13);
  font.setWeight(QFont::Bold);
  headerItem->setFont(font);
  headerItem->setPos(indent, indent);

  textItem = new QGraphicsTextItem(this);
  textItem->setDefaultTextColor(Colors::White);
  font = textItem->font();
  font.setPixelSize(11);
  font.setWeight(QFont::Medium);
  textItem->setFont(font);
  textItem->setPos(
    headerItem->mapToParent(headerItem->boundingRect().bottomLeft())
    );

  update();
}

void Tooltip::setHeader(const QString& newHeader)
{
  header = newHeader;
  update();
}

void Tooltip::setText(const QString& newText)
{
  text = newText;
  update();
}

void Tooltip::update()
{
  auto parent = this;
  headerItem->setPlainText(header);
  if (header.isEmpty())
  {
    parent = nullptr;
  }
  headerItem->setParentItem(parent);

  parent = this;
  textItem->setPlainText(text);
  if (text.isEmpty())
  {
    parent = nullptr;
  }
  textItem->setParentItem(parent);

  setSize(childrenBoundingRect().size() + QSize(2 * indent, 2 * indent));
}

} }