#include "panel.h"
#include "qgraphicssceneevent.h"
#include <memory>
#include <QDebug>
#include "../../model/project.h"
#include "colors.h"

namespace Pipeline { namespace View {

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
      auto node = nodeMap[step];
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

Panel::Panel(QObject* parent)
  : QGraphicsScene(parent)
{
  setBackgroundBrush(QBrush(Colors::Egg));
}

void Panel::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
  if (activeConnector)
  {
    activeConnector->redraw(event->scenePos());
  }
  QGraphicsScene::dragMoveEvent(event);
}

Connector* Panel::getActiveConnector()
{
  return activeConnector;
}

} }
