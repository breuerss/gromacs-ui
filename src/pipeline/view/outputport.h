#ifndef PIPELINE_OUTPUTPORT_H
#define PIPELINE_OUTPUTPORT_H

#include "port.h"
#include "../../command/types.h"

namespace Pipeline { namespace View {

class OutputPort : public Port
{
  Q_OBJECT
public:
  OutputPort(double x, double y, QGraphicsItem* parent = nullptr);
  virtual ~OutputPort();

  void setProvidedData(const Command::Data& data);
  const Command::Data& getProvidedData() const { return data; }

  Q_PROPERTY(double scale READ scale WRITE setScale NOTIFY scaleChanged);
  void setScale(double scale);

protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent*) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

signals:
  void scaleChanged();
  void clicked();

private:
  bool hasData();

  Command::Data data;
  QMetaObject::Connection conn;
  QPropertyAnimation* increaseSize;
};

} }

#endif
