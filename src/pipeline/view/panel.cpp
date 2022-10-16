#include "panel.h"
#include "colors.h"
#include "connector.h"
#include "node.h"
#include "../../model/project.h"
#include "../../undoredo/stack.h"
#include "src/pipeline/step.h"
#include "src/pipeline/view/outputport.h"

#include <memory>
#include <QDebug>
#include <QMarginsF>
#include <numeric>

namespace Pipeline { namespace View {

Panel::Panel(QObject* parent)
  : QGraphicsScene(parent)
{
  setBackgroundBrush(QBrush(Colors::DarkGrey));
  setSceneRect(QRect(0, 0, 0, 0) + QMarginsF(1, 1, 1, 1) * 5000);

  connect(this, &QGraphicsScene::selectionChanged, [this] () {
    auto items = selectedItems();
    if (items.size() == 1)
    {
      firstSelectedNode = dynamic_cast<Node*>(items[0]);
    }
    else if (items.size() == 0)
    {
      firstSelectedNode = nullptr;
    }
  });
}

Panel::~Panel()
{
  for (auto conn : conns)
  {
    disconnect(conn);
  }
}

void Panel::reuseConnectorFor(Port* port)
{
  stopConnector();

  auto connector = takeConnectorFor(port);
  connector->setEndingPort(nullptr);
  startingNode = connector->getStartingPort()->parentItem();

  activeConnector = connector;
}

void Panel::startConnector(OutputPort* at)
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

void Panel::addConnector(const NodePair& pair)
{
  if (!connectorMap.contains(pair))
  {
    auto connector = new Connector(pair.start);
    connector->setEndingPort(pair.end);
    addItem(connector);
    connectorMap.insert(pair, connector);
  }
}

void Panel::removeConnectorFor(const NodePair& pair)
{
  if (pair && connectorMap.contains(pair))
  {
    auto connector = connectorMap.take(pair);
    removeItem(connector);
    delete connector;
  }
}

Connector* Panel::takeConnectorFor(Port* port)
{
  Connector* connector = nullptr;

  auto found = getPairFor(port);
  if (found)
  {
    connector = connectorMap.take(found);
  }

  return connector;
}

Connector* Panel::getConnectorFor(Port* port)
{
  Connector* connector = nullptr;

  auto found = getPairFor(port);
  if (found)
  {
    connector = connectorMap[found];
  }

  return connector;
}

Panel::NodePair Panel::getPairFor(Port* port)
{
  NodePair foundPair;
  for (const auto& pair : connectorMap.keys())
  {
    if (pair.start == port || pair.end == port)
    {
      foundPair = pair;
      break;
    }
  }

  return foundPair;
}

void Panel::setProject(std::shared_ptr<Model::Project> newProject)
{
  for (auto conn: conns)
  {
    disconnect(conn);
  }

  outputPortMap.clear();
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

void Panel::addOutputPort(const Command::Data& data, OutputPort* port)
{
  outputPortMap[data] = port;
}

OutputPort* Panel::getOutputPort(const Command::Data& data)
{
  return outputPortMap[data];
}

void Panel::deleteSelectedNodes()
{
  UndoRedo::Stack::getInstance()->beginMacro("Delete selected nodes");
  for (auto item: selectedItems())
  {
    auto node = dynamic_cast<Node*>(item);
    project->removeStep(node->getStep());
  }
  clearSelection();
  UndoRedo::Stack::getInstance()->endMacro();
}

void Panel::setAllNodesSelected(bool selected)
{
  for (auto node: nodeMap.values())
  {
    node->setSelected(selected);
  }
}

QList<Node*> Panel::getSelectedNodes() const
{
  QList<Node*> selectedNodes;
  auto items = selectedItems();
  for (auto item : items)
  {
    selectedNodes << dynamic_cast<Node*>(item);
  }

  return selectedNodes;
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
          (selectedItems().size() - 1);
        newPos.setX(prevNodeInScene.right() + stepSize);
      }
      else
      {
        auto firstNodeTop = firstNodeInScene.top();
        auto lastNodeBottom = lastNodeInScene.bottom();
        auto nodeHeight = firstNode->rect().height();
        const double stepSize = (lastNodeBottom - firstNodeTop - selectedItems().size() * nodeHeight) /
          (selectedItems().size() - 1);
        newPos.setY(prevNodeInScene.bottom() + stepSize);
      }
      node->setPos(newPos);
    });
}

void Panel::alignSelectedNodes(Panel::Alignment alignment)
{
  execOnSelectedNodesGroup([this, alignment] (Node* node, int, QList<Node*>) {
    if (node == firstSelectedNode)
    {
      return;
    }

    auto firstNodeInScene = firstSelectedNode->mapRectToScene(firstSelectedNode->rect()).toRect();
    auto nodeRect = node->rect().toRect();
    auto newPos = node->pos();
    switch(alignment)
    {
      case Alignment::Left:
        newPos.setX(firstNodeInScene.x());
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
    if (node->scene())
    {
      removeItem(node);
      node->setSelected(false);
    }
    delete node;
  }
}

void Panel::addNode(std::shared_ptr<Pipeline::Step> step)
{
  auto node = new Node(step);
  nodeMap[step] = node;
  addItem(node);
  for (const auto& pair : node->getOutputPorts())
  {
    addOutputPort(pair.first, pair.second);
  }

  auto pos = step->getLocation();
  if (pos.isNull())
  {
    pos = itemsBoundingRect().center();
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
  QGraphicsScene::mouseReleaseEvent(event);

  if (startingPos != event->scenePos() && movingNode)
  {
    nodeMoved(movingNode, nodePos);
  }
}

Connector* Panel::getActiveConnector()
{
  return activeConnector;
}

} }
