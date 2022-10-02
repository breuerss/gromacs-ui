#ifndef PIPELINE_INPUTPORT_H
#define PIPELINE_INPUTPORT_H

#include "port.h"

namespace Pipeline { namespace View {

class InputPort : public Port
{
  Q_OBJECT
public:
  InputPort(double x, double y, QGraphicsItem* parent = nullptr);

  void setAcceptedFileTypes(const QList<Command::FileObject::Type>& newAcceptedFileTypes);
  const QList<Command::FileObject::Type>& getAcceptedFileTypes() const;

protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
  void dropEvent(QGraphicsSceneDragDropEvent *event) override;

private:
  Command::Data getData();
  QList<Command::FileObject::Type> acceptedFileTypes;
};

} }

#endif
