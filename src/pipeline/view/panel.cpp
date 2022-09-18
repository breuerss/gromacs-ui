#include "panel.h"
#include "colors.h"
#include "connector.h"
#include "node.h"
#include "../../model/project.h"

#include <memory>
#include <QDebug>
#include <QMarginsF>

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
  addItem(activeConnector);
}

void Panel::addConnector(Port* start, Port* end)
{
  auto deletePortEntry = [this] (Port* deleted) {
    deleteConnectorFor(deleted);
    disconnect(QObject::sender());
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
  removeItem(connector);
  delete connector;
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
      auto node = nodeMap.take(step);
      // delete node before removal to have access
      // to scene() inside of destructor of port
      delete node;
      removeItem(node);
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
  for (auto node: nodeMap.values())
  {
    if (node->isSelected())
    {
      project->removeStep(node->getStep());
    }
  }
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
  for (auto node: nodeMap.values())
  {
    if (node->isSelected())
    {
      selectedNodes << node;
    }
  }

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
  QList<Node*> selectedNodes = getSelectedNodes();
  for (auto node: selectedNodes)
  {
    node->setY(node->y() + y);
  }
}

void Panel::moveSelectedNodesHorizontal(int x)
{
  QList<Node*> selectedNodes = getSelectedNodes();
  for (auto node: selectedNodes)
  {
    node->setX(node->x() + x);
  }
}

void Panel::distributeSelectedNodes(Distribution alignment)
{
  execOnSelectedNodesGroup([alignment] (auto node, auto group, int index, auto selectedNodes) {
    QRectF box = group->boundingRect();
    if (alignment == Distribution::Horizontal)
    {
      const auto stepSize = (box.width() -
                             selectedNodes[selectedNodes.size() - 1]->boundingRect().width()) /
        (selectedNodes.size() - 1);
      node->setX(box.x() + stepSize * index);
    }
    else
    {
      const auto stepSize = (box.height() -
                             selectedNodes[selectedNodes.size() - 1]->boundingRect().height()) /
        (selectedNodes.size() - 1);
      node->setY(box.y() + stepSize * index);
    }
  });
}

void Panel::execOnSelectedNodesGroup(
  std::function<void(Node*, QGraphicsItemGroup* group, int, const QList<Node*>)> callback)
{
  QList<Node*> selectedNodes = getSelectedNodes();

  auto group = new QGraphicsItemGroup();
  for (auto node: selectedNodes)
  {
    group->addToGroup(node);
  }

  for (int index = 0; index < selectedNodes.size(); index++)
  {
    callback(selectedNodes[index], group, index, selectedNodes);
  }

  for (auto node: selectedNodes)
  {
    addItem(node);
  }
}

void Panel::alignSelectedNodes(Panel::Alignment alignment)
{
  execOnSelectedNodesGroup([alignment] (auto node, auto group, int, auto) {
    QRectF box = group->boundingRect();
    switch(alignment)
    {
      case Alignment::Left:
        node->setX(box.x());
        break;
      case Alignment::Center:
        node->setX(box.center().x() - node->boundingRect().width() / 2);
        break;
      case Alignment::Right:
        node->setX(box.x() + box.width() - node->boundingRect().width());
        break;
      case Alignment::Top:
        node->setY(box.y());
        break;
      case Alignment::Middle:
        node->setY(box.center().y() - node->boundingRect().height() / 2);
        break;
      case Alignment::Bottom:
        node->setY(box.y() + box.height() - node->boundingRect().height());
        break;
      default:
        break;
    }
  });

}

void Panel::addNode(std::shared_ptr<Pipeline::Step> step)
{
  auto node = new Node(step);
  nodeMap[step] = node;
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

void Panel::stopConnector()
{
  if (activeConnector)
  {
    removeItem(activeConnector);
    delete activeConnector;
    activeConnector = nullptr;
  }
}

void Panel::connectorAccepted()
{
  addConnector(activeConnector->getStartingPort(), activeConnector->getEndingPort());
  stopConnector();
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
  startingPos = event->screenPos();
}

void Panel::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if (
    !event->modifiers().testFlag(Qt::ShiftModifier) &&
    !event->modifiers().testFlag(Qt::ControlModifier) &&
    startingPos == event->screenPos()
    )
  {
    setAllNodesSelected(false);
  }
  QGraphicsScene::mouseReleaseEvent(event);
}

Connector* Panel::getActiveConnector()
{
  return activeConnector;
}

} }
