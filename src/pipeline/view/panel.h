#ifndef PIPELINE_PANEL_H
#define PIPELINE_PANEL_H

#include "connector.h"
#include "node.h"
#include <QGraphicsScene>
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

class Panel : public QGraphicsScene
{

public:
  Panel(QObject* parent = nullptr);

  void setProject(std::shared_ptr<Model::Project> newProject);
  std::shared_ptr<Model::Project> getProject() const;

  void reuseConnector(Connector* connector);
  void startConnector(Port* at);
  void stopConnector();
  void connectorAccepted();
  Connector* getActiveConnector();
  QGraphicsItem* startingNode = nullptr;

protected:
  void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
  Connector* activeConnector = nullptr;

private:
  std::shared_ptr<Model::Project> project;
  QList<QMetaObject::Connection> conns;
  void addNode(std::shared_ptr<Pipeline::Step> step);
  std::map<std::shared_ptr<Pipeline::Step>, Node*> modelViewMap;
};

} }

#endif
