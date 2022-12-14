#ifndef PIPELINE_ROUNDED_RECTANGLE_H
#define PIPELINE_ROUNDED_RECTANGLE_H

#include <QGraphicsRectItem>
#include <QPainter>
#include <QObject>

namespace Pipeline { namespace View {

class RoundedRectItem : public QObject, public QGraphicsRectItem {

  Q_OBJECT
public:
  using QGraphicsRectItem::QGraphicsRectItem;

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

  ///Get the rounded rect corner x radius
  double getRadiusX() const { return yRadius; }

  ///Get the rounded rect corner y radius
  double getRadiusY() const { return yRadius; }

  ///Set the rounded rect corner x radius
  void setRadiusX(const double newXRadius) { xRadius = newXRadius; }

  ///Set the rounded rect corner y radius
  void setRadiusY(const double newYRadius) { yRadius = newYRadius; }

  ///Set the rounded rect
  void setRoundedRect(const QRectF rect, const double radius_x, const double radius_y);
  void setRadius(const double radius);
  void setSize(const QSizeF& size);
  QSizeF getSize() const;
  
  Q_PROPERTY(QSizeF size READ getSize WRITE setSize);
private:
  ///The rounded rect corner x radius
  double xRadius;

  ///The rounded rect corner y radius
  double yRadius;
};

} }

#endif
