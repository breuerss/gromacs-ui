#ifndef PIPELINE_PANEL_H
#define PIPELINE_PANEL_H

#include "../../command/types.h"
#include <QGraphicsScene>
#include <QMap>
#include <QList>
#include <QMetaObject>
#include <QGraphicsSceneMouseEvent>
#include <memory>
#include <tuple>

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

  struct NodePair {
    OutputPort* start = nullptr;
    InputPort* end = nullptr;
    operator bool () const
    {
      return start && end;
    }
    bool operator<(const NodePair& other) const {
      return std::tie(start, end) < std::tie(other.start, other.end);
    }
  };

  Panel(QObject* parent = nullptr);
  ~Panel();

  void setProject(std::shared_ptr<Model::Project> newProject);
  std::shared_ptr<Model::Project> getProject() const;

  void reuseConnectorFor(Port* port);
  void startConnector(OutputPort* at);
  void addConnector(const NodePair& nodePair);
  void stopConnector();
  Connector* getActiveConnector();
  QGraphicsItem* startingNode = nullptr;

  OutputPort* getOutputPort(const Command::Data& data);
  void removeConnectorFor(const NodePair& pair);
  Connector* getConnectorFor(Port* port);
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

  Connector* takeConnectorFor(Port* port);
  NodePair getPairFor(Port* port);
  void setNodeSelected(Node* node, bool selected);
  void addNode(std::shared_ptr<Pipeline::Step> step);
  void removeNode(std::shared_ptr<Pipeline::Step> step);
  void addOutputPort(const Command::Data&, OutputPort*);
  QList<Node*> getSortedSelectedNodes() const;
  QList<Node*> getSelectedNodes() const;
  Node* firstSelectedNode = nullptr;

  QMap<std::shared_ptr<Pipeline::Step>, Node*> nodeMap;
  std::map<Command::Data, OutputPort*> outputPortMap;
  QMap<NodePair, Connector*> connectorMap;
  QPointF startingPos;
  Node* movingNode = nullptr;
  QPointF nodePos;
};

} }

#endif
