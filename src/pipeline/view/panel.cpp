#include "panel.h"
#include "colors.h"
#include "connector.h"
#include "node.h"
#include "../../model/project.h"
#include "../../undoredo/stack.h"

#include <memory>
#include <QDebug>
#include <QMarginsF>
#include <numeric>

namespace Pipeline { namespace View {

Panel::Panel(QObject* parent)
  : QGraphicsScene(parent)
{
  setBackgroundBrush(QBrush(Colors::DarkGrey));
  setSceneRect(itemsBoundingRect() + QMarginsF(1, 1, 1, 1) * 5000);
}

Panel::~Panel()
{
  for (auto conn : conns)
  {
    disconnect(conn);
  }
}

void Panel::reuseConnector(Connector* connector)
{
  stopConnector();

  connector->setEndingPort(nullptr);
  startingNode = connector->getStartingPort()->parentItem();

  activeConnector = connector;
}

void Panel::startConnector(Port* at)
{
  stopConnector();
  activeConnector = new Connector(at);
  startingNode = at->parentItem();
  addItem(activeConnector);
}

void Panel::stopConnector()
{
  if (activeConnector)
  {
    removeItem(activeConnector);
    delete activeConnector;
    activeConnector = nullptr;
    startingNode = nullptr;
  }
}

void Panel::connectorAccepted()
{
  addConnector(activeConnector->getStartingPort(), activeConnector->getEndingPort());
  stopConnector();
}

void Panel::addConnector(Port* start, Port* end)
{
  auto deletePortEntry = [this] (Port* deleted) {
    deleteConnectorFor(deleted);
    disconnect(deleted);
  };
  conns << connect(start, &Port::deleted, deletePortEntry);
  conns << connect(end, &Port::deleted, deletePortEntry);
  if (!connectorMap.count({start, end}))
  {
    auto connector = new Connector(start);
    connector->setEndingPort(end);
    addItem(connector);
    connectorMap.insert({ start, end }, connector);
  }
}

void Panel::deleteConnectorFor(Port* port)
{
  auto connector = getConnectorFor(port);
  if (connector)
  {
    removeItem(connector);
    delete connector;
  }
}

Connector* Panel::getConnectorFor(Port* port)
{
  Connector* connector = nullptr;

  for (const auto& pair : connectorMap.keys())
  {
    if (pair.second == port || pair.first == port)
    {
      connector = connectorMap.take(pair);
      break;
    }
  }

  return connector;
}

void Panel::setProject(std::shared_ptr<Model::Project> newProject)
{
  for (auto conn: conns)
  {
    disconnect(conn);
  }

  portMap.clear();
  connectorMap.clear();
  nodeMap.clear();
  clear();

  project = newProject;

  for (auto step : project->getSteps())
  {
    addNode(step);
  }

  conns << connect(
    project.get(), &Model::Project::stepAdded,
    [this] (const auto& step) {
      addNode(step);
    });

  conns << connect(
    project.get(), &Model::Project::stepRemoved,
    [this] (const auto& step) {
      removeNode(step);
    });
}

std::shared_ptr<Model::Project> Panel::getProject() const
{
  return project;
}

void Panel::addPort(std::shared_ptr<Command::FileObject> fileObject, Port* port)
{
  portMap[fileObject] = port;
}

Port* Panel::getPort(std::shared_ptr<Command::FileObject> fileObject)
{
  return portMap[fileObject];
}

void Panel::deleteSelectedNodes()
{
  for (auto node: nodeSelection)
  {
    project->removeStep(node->getStep());
  }
  nodeSelection.clear();

  selectedNodesChanged(getSelectedNodes());
}

void Panel::setAllNodesSelected(bool selected)
{
  for (auto node: nodeMap.values())
  {
    node->setSelected(selected);
  }
}

const QList<Node*>& Panel::getSelectedNodes() const
{
  return nodeSelection;
}

QList<Node*> Panel::getSortedSelectedNodes() const
{
  auto selectedNodes = getSelectedNodes();

  std::sort(
    selectedNodes.begin(), selectedNodes.end(),
    [] (Node* first, Node* second) {
      return first->x() < second->x();
    });
  std::sort(
    selectedNodes.begin(), selectedNodes.end(),
    [] (Node* first, Node* second) {
      return first->y() < second->y();
    });

  return selectedNodes;
}

void Panel::moveSelectedNodesVertical(int y)
{
  UndoRedo::Stack::getInstance()->beginMacro("Move vertically");
  QList<Node*> selectedNodes = getSelectedNodes();
  for (auto node: selectedNodes)
  {
    auto oldPos = node->pos();
    node->setY(node->y() + y);
    nodeMoved(node, oldPos);
  }
  UndoRedo::Stack::getInstance()->endMacro();
}

void Panel::moveSelectedNodesHorizontal(int x)
{
  UndoRedo::Stack::getInstance()->beginMacro("Move horizontally");
  QList<Node*> selectedNodes = getSelectedNodes();
  for (auto node: selectedNodes)
  {
    auto oldPos = node->pos();
    node->setX(node->x() + x);
    nodeMoved(node, oldPos);
  }
  UndoRedo::Stack::getInstance()->endMacro();
}

void Panel::execOnSelectedNodesGroup(
  std::function<void(Node*, int, const QList<Node*>&)> callback)
{
  UndoRedo::Stack::getInstance()->beginMacro("Align nodes");
  auto sortedNodes = getSortedSelectedNodes();
  for (int index = 0; index < sortedNodes.size(); index++)
  {
    auto node = sortedNodes[index];
    auto oldPos = node->pos();
    callback(node, index, sortedNodes);
    nodeMoved(node, oldPos);
  }
  UndoRedo::Stack::getInstance()->endMacro();
}

void Panel::distributeSelectedNodes(Distribution alignment)
{
  execOnSelectedNodesGroup(
    [this, alignment] (Node* node, int index, const QList<Node*>& sortedNodes) {
      auto firstNode = sortedNodes.first();
      if (node == firstNode)
      {
        return;
      }

      auto firstNodeInScene = firstNode->mapRectToScene(firstNode->rect());
      auto lastNode = sortedNodes.last();
      auto lastNodeInScene = lastNode->mapRectToScene(lastNode->rect());
      auto prevNode = sortedNodes[index - 1];
      auto prevNodeInScene = prevNode->mapRectToScene(prevNode->rect());
      auto newPos = node->pos();
      if (alignment == Distribution::Horizontal)
      {
        auto firstNodeLeft = firstNodeInScene.left();
        auto lastNodeRight = lastNodeInScene.right();
        double sumWidth = 0;
        for (auto sortedNode : sortedNodes)
        {
          sumWidth += sortedNode->rect().width();
        }
        const auto stepSize = (lastNodeRight - firstNodeLeft - sumWidth) /
          (nodeSelection.size() - 1);
        newPos.setX(prevNodeInScene.right() + stepSize);
      }
      else
      {
        auto firstNodeTop = firstNodeInScene.top();
        auto lastNodeBottom = lastNodeInScene.bottom();
        auto nodeHeight = firstNode->rect().height();
        const double stepSize = (lastNodeBottom - firstNodeTop - nodeSelection.size() * nodeHeight) /
          (nodeSelection.size() - 1);
        newPos.setY(prevNodeInScene.bottom() + stepSize);
      }
      node->setPos(newPos);
    });
}

void Panel::alignSelectedNodes(Panel::Alignment alignment)
{
  execOnSelectedNodesGroup([this, alignment] (Node* node, int, QList<Node*>) {
    auto firstNode = nodeSelection[0];
    if (node == firstNode)
    {
      return;
    }

    auto firstNodeInScene = firstNode->mapRectToScene(firstNode->rect()).toRect();
    auto nodeRect = node->rect().toRect();
    auto newPos = node->pos();
    switch(alignment)
    {
      case Alignment::Left:
        newPos.setX(firstNode->x());
        break;
      case Alignment::Center:
        {
          auto center = firstNodeInScene.center();
          newPos.setX(center.x() - std::floor(nodeRect.width() / 2.));
          break;
        }
      case Alignment::Right:
        {
          auto right = firstNodeInScene.right();
          newPos.setX(right - nodeRect.width());
          break;
        }
      case Alignment::Top:
        newPos.setY(firstNodeInScene.top());
        break;
      case Alignment::Middle:
        {
          auto center = firstNodeInScene.center();
          newPos.setY(center.y() - std::floor(nodeRect.height() / 2.));
          break;
        }
      case Alignment::Bottom:
        {
          auto bottom = firstNodeInScene.bottom();
          newPos.setY(bottom - nodeRect.height());
          break;
        }
      default:
        break;
    }

    node->setPos(newPos);
  });

}

void Panel::removeNode(std::shared_ptr<Pipeline::Step> step)
{
  auto node = nodeMap.take(step);

  if (node)
  {
    // delete node before removal to have access
    // to scene() inside of destructor of port
    disconnect(node);
    delete node;
    removeItem(node);
  }
}

void Panel::addNode(std::shared_ptr<Pipeline::Step> step)
{
  auto node = new Node(step);
  nodeMap[step] = node;
  connect(node, &Node::selectedChanged, [this, node] () {
    if (node->isSelected())
    {
      nodeSelection << node;
    }
    else
    {
      nodeSelection.removeOne(node);
    }
    selectedNodesChanged(getSelectedNodes());
  });
  addItem(node);
  for (const auto& pair : node->getOutputPorts())
  {
    addPort(pair.first, pair.second);
  }

  auto pos = step->location.topLeft();
  if (!step->location.isValid())
  {
    pos = itemsBoundingRect().topRight() + QPointF(20, 0);
  }
  node->setPos(pos);
}

void Panel::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
  if (activeConnector)
  {
    activeConnector->redraw(event->scenePos());
  }
  QGraphicsScene::dragMoveEvent(event);
}

void Panel::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsScene::mousePressEvent(event);
  startingPos = event->scenePos();

  const QList<QGraphicsItem*> itemList = items(startingPos,
                                               Qt::IntersectsItemShape,
                                               Qt::AscendingOrder);
  movingNode = dynamic_cast<Node*>(itemList.isEmpty() ? nullptr : itemList.first());
  if (movingNode)
  {
    nodePos = movingNode->pos();
  }
}

void Panel::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if (
    !event->modifiers().testFlag(Qt::ShiftModifier) &&
    !event->modifiers().testFlag(Qt::ControlModifier) &&
    startingPos == event->scenePos()
    )
  {
    setAllNodesSelected(false);
  }

  if (startingPos != event->scenePos())
  {
    nodeMoved(movingNode, nodePos);
  }

  QGraphicsScene::mouseReleaseEvent(event);
}

Connector* Panel::getActiveConnector()
{
  return activeConnector;
}

} }
