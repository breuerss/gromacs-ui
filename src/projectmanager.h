#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>
#include <memory>
#include "project.h"

class ProjectManager : public QObject
{
    Q_OBJECT
public:
    static ProjectManager* getInstance();
    const std::shared_ptr<Project> getCurrentProject() const;

signals:
    void currentProjectChanged(const Project& currentProject);

public slots:
    void addStep();

private:
    ProjectManager() = default;
    std::shared_ptr<Project> currentProject;
};

#endif // PROJECTMANAGER_H
