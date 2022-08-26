#include "panel.h"
#include "qgraphicssceneevent.h"
#include <memory>
#include <QDebug>
#include "../../model/project.h"
#include "src/pipeline/supportedsteps.h"

namespace Pipeline { namespace View {

void Panel::reuseConnector(Connector* connector)
{
  stopConnector();
  activeConnector = connector;
}

void Panel::startConnector(Port* at)
{
  stopConnector();
  activeConnector = new Connector(at);
  addItem(activeConnector);
}

void Panel::setProject(std::shared_ptr<Model::Project> newProject)
{
  for (auto conn: conns)
  {
    disconnect(conn);
  }

  modelViewMap.clear();
  clear();

  project = newProject;

  for (auto step : project->getSteps())
  {
    std::visit([this] (auto step) { addNode(step); }, step);
  }

  conns << connect(
    project.get(), &Model::Project::stepAdded,
    [this] (const Pipeline::StepType& step) {
      std::visit([this] (auto step) { addNode(step); }, step);
    });
}

std::shared_ptr<Model::Project> Panel::getProject() const
{
  return project;
}

void Panel::addNode(std::shared_ptr<Pipeline::Step> step)
{
  auto node = new Node(step);
  modelViewMap[step] = node;
  addItem(node);
  node->setPos(itemsBoundingRect().topRight() + QPointF(20, 0));
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
  activeConnector = nullptr;
}

Panel::Panel(QObject* parent)
  : QGraphicsScene(parent)
{
  setBackgroundBrush(QBrush("#cccccc"));
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
