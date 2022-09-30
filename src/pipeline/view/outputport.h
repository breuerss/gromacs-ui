#ifndef PIPELINE_OUTPUTPORT_H
#define PIPELINE_OUTPUTPORT_H

#include "port.h"

namespace Pipeline { namespace View {

class OutputPort : public Port
{
  Q_OBJECT
public:
  OutputPort(double x, double y, QGraphicsItem* parent = nullptr);
  virtual ~OutputPort();

  void setProvidedFileObject(std::shared_ptr<Command::FileObject> fileObject);
  std::shared_ptr<Command::FileObject> getFileObject() const { return fileObject; }

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

  std::shared_ptr<Command::FileObject> fileObject;
  QMetaObject::Connection conn;
  QPropertyAnimation* increaseSize;
};

} }

#endif
