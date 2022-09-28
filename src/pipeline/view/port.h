#ifndef PIPELINE_PORT_H
#define PIPELINE_PORT_H

#include "../../command/fileobject.h"
#include "porttooltip.h"
#include <QGraphicsEllipseItem>
#include <memory>
#include <qpropertyanimation.h>

namespace Pipeline { namespace View {

class Port : public QObject, public QGraphicsEllipseItem
{
  Q_OBJECT
public:
  const static unsigned int RADIUS = 8;
  enum class Type {
    Input,
    Output,
  };
  Port(double x, double y, Type type, QGraphicsItem* parent = nullptr);
  virtual ~Port();

  QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value) override;
  QPointF getCenterInScene() const;
  void setConnected(bool connected = true);

  std::shared_ptr<Command::FileObject> getFileObject() const { return fileObject; }
  void setProvidedFileObject(std::shared_ptr<Command::FileObject> fileObject);
  void setCategory(Command::FileObject::Category category);
  void setAcceptedFileTypes(const QList<Command::FileObject::Type>& newAcceptedFileTypes);
  const QList<Command::FileObject::Type>& getAcceptedFileTypes() const { return acceptedFileTypes; }

  QSizeF getSize() const;
  void setSize(const QSizeF& newSize);

  Q_PROPERTY(QSizeF size READ getSize WRITE setSize NOTIFY sizeChanged);
protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
  void dropEvent(QGraphicsSceneDragDropEvent *event) override;
  void hoverEnterEvent(QGraphicsSceneHoverEvent*) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override;

signals:
  void deleted(Port*);
  void centerPositionChanged(const QPointF& center);
  void connectedToChanged(
    std::shared_ptr<Command::FileObject> newFileObject,
    std::shared_ptr<Command::FileObject> oldFileObject
    );
  void clicked();
  void sizeChanged();

private:
  void setupTooltip();
  bool hasData();

  Type type;
  bool connected = false;
  std::shared_ptr<Command::FileObject> fileObject;
  QMetaObject::Connection conn;
  QList<Command::FileObject::Type> acceptedFileTypes;
  QPointF startingPos;
  PortTooltip* tooltipBox;
  QPropertyAnimation* increaseSize;
};

} }

#endif
