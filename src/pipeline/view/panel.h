#ifndef PIPELINE_PANEL_H
#define PIPELINE_PANEL_H

#include "src/command/fileobject.h"
#include <QGraphicsScene>
#include <QMap>
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

class Panel : public QGraphicsScene
{

public:
  Panel(QObject* parent = nullptr);
  ~Panel();

  void setProject(std::shared_ptr<Model::Project> newProject);
  std::shared_ptr<Model::Project> getProject() const;

  void reuseConnector(Connector* connector);
  void startConnector(Port* at);
  void addConnector(Port* start, Port* end);
  void stopConnector();
  void connectorAccepted();
  Connector* getActiveConnector();
  QGraphicsItem* startingNode = nullptr;

  Port* getPort(std::shared_ptr<Command::FileObject>);
  void deleteConnectorFor(Port* port);
  Connector* getConnectorFor(Port* port);

protected:
  void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
  Connector* activeConnector = nullptr;

private:
  std::shared_ptr<Model::Project> project;
  QList<QMetaObject::Connection> conns;
  void addNode(std::shared_ptr<Pipeline::Step> step);
  void addPort(std::shared_ptr<Command::FileObject>, Port*);
  std::map<std::shared_ptr<Pipeline::Step>, Node*> nodeMap;
  std::map<std::shared_ptr<Command::FileObject>, Port*> portMap;
  QMap<QPair<Port*, Port*>, Connector*> connectorMap;
};

} }

#endif
