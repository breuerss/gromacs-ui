#ifndef PIPELINE_PANEL_H
#define PIPELINE_PANEL_H

#include "../../command/fileobject.h"
#include <QGraphicsScene>
#include <QMap>
#include <QList>
#include <QMetaObject>
#include <QGraphicsSceneMouseEvent>
#include <memory>

namespace Pipeline {
class Step;
}

namespace Model {
class Project;
}

namespace Pipeline { namespace View {

class Node;
class Connector;
class Port;
class OutputPort;
class InputPort;

class Panel : public QGraphicsScene
{
  Q_OBJECT
public:
  enum class Alignment {
    Unknown = 0,
    Left,
    Right,
    Center,
    Top,
    Bottom,
    Middle,
  };
  enum class Distribution {
    Unknown = 0,
    Horizontal,
    Vertical,
  };
  Panel(QObject* parent = nullptr);
  ~Panel();

  void setProject(std::shared_ptr<Model::Project> newProject);
  std::shared_ptr<Model::Project> getProject() const;

  void reuseConnector(Connector* connector);
  void startConnector(OutputPort* at);
  void addConnector(OutputPort* start, InputPort* end);
  void stopConnector();
  void connectorAccepted();
  Connector* getActiveConnector();
  QGraphicsItem* startingNode = nullptr;

  OutputPort* getOutputPort(std::shared_ptr<Command::FileObject>);
  void deleteConnectorFor(Port* port);
  Connector* getConnectorFor(Port* port);
  const QList<Node*>& getSelectedNodes() const;
  void deleteSelectedNodes();
  void alignSelectedNodes(Alignment alignment);
  void distributeSelectedNodes(Distribution alignment);
  void moveSelectedNodesHorizontal(int x);
  void moveSelectedNodesVertical(int x);
  void setAllNodesSelected(bool selected);

  void execOnSelectedNodesGroup(
    std::function<void(Node*, int, const QList<Node*>&)> callback);

signals:
  void selectedNodesChanged(QList<Node*>);
  void nodeMoved(Node* node, const QPointF oldPosition);

protected:
  void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  Connector* activeConnector = nullptr;

private:
  std::shared_ptr<Model::Project> project;
  QList<QMetaObject::Connection> conns;
  void addNode(std::shared_ptr<Pipeline::Step> step);
  void removeNode(std::shared_ptr<Pipeline::Step> step);
  void addOutputPort(std::shared_ptr<Command::FileObject>, OutputPort*);
  QList<Node*> getSortedSelectedNodes() const;

  QMap<std::shared_ptr<Pipeline::Step>, Node*> nodeMap;
  std::map<std::shared_ptr<Command::FileObject>, OutputPort*> outputPortMap;
  QMap<QPair<Port*, Port*>, Connector*> connectorMap;
  QList<Node*> nodeSelection;
  QPointF startingPos;
  Node* movingNode = nullptr;
  QPointF nodePos;
};

} }

#endif
