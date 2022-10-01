#include "tooltip.h"
#include "colors.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>

namespace Pipeline { namespace View {

Tooltip::Tooltip(QGraphicsItem* parent)
  : RoundedRectItem(parent)
{
  setAcceptedMouseButtons(Qt::NoButton);
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

Tooltip::~Tooltip()
{
  delete headerItem;
  delete textItem;
}

QVariant Tooltip::itemChange(GraphicsItemChange change, const QVariant &value)
{
  if (change == QGraphicsItem::ItemVisibleChange)
  {
    if (value.toBool())
    {
      auto viewer = scene()->views()[0];
      auto sceneScale = viewer->transform().m11();
      setScale(1 / sceneScale);

      QRectF viewport = viewer->viewport()->rect();
      auto tooFar= viewer->mapToScene(viewport.toRect().bottomRight()) -
        mapToScene(rect().bottomRight());
      auto tooltipPos = pos();
      if (tooFar.x() < 0)
      {
        tooltipPos.rx() += tooFar.x() - 20;
      }
      if (tooFar.y() < 0)
      {
        tooltipPos.ry() += tooFar.y() - 20;
      }
      setPos(tooltipPos);
    }
  }

  return QGraphicsItem::itemChange(change, value);
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
  textItem->setHtml(text);
  if (text.isEmpty())
  {
    parent = nullptr;
  }
  textItem->setParentItem(parent);

  auto size = childrenBoundingRect().size();
  if (!size.isEmpty())
  {
    size += QSize(2 * indent, 2 * indent);
  }
  setSize(size);
}

} }
