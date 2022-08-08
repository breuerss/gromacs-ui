#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>
#include <memory>

namespace Model {
class Project;
}

class ProjectManager : public QObject
{
  Q_OBJECT
public:
  static ProjectManager* getInstance();
  const std::shared_ptr<Model::Project> getCurrentProject() const;
  void createNewProject();

signals:
  void currentProjectChanged(std::shared_ptr<Model::Project> currentProject);

  public slots:
  void save();
  void open();
  void saveAs(const QString& fileName = "");

private:
  ProjectManager() = default;
  std::shared_ptr<Model::Project> currentProject;
  QString fileName;
};

#endif // PROJECTMANAGER_H
