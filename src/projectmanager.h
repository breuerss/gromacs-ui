#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "project.h"

#include <QObject>
#include <memory>

class ProjectManager : public QObject
{
    Q_OBJECT
public:
    static ProjectManager* getInstance();
    const std::shared_ptr<Project> getCurrentProject() const;
    void createNewProject();

signals:
    void currentProjectChanged(std::shared_ptr<Project> currentProject);

public slots:
    void addStep();
    void save();
    void open();
    void saveAs(const QString& fileName = "");

private:
    ProjectManager() = default;
    std::shared_ptr<Project> currentProject;
    QString fileName;
};

#endif // PROJECTMANAGER_H
