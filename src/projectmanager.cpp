#include "projectmanager.h"

#include <memory>

ProjectManager* ProjectManager::getInstance()
{
    static std::unique_ptr<ProjectManager> instance;
    if (!instance)
    {
        instance.reset(new ProjectManager());
    }

    return instance.get();
}

void ProjectManager::addStep()
{
    if (currentProject)
    {
        currentProject->addStep();
    }
}

const std::shared_ptr<Project> ProjectManager::getCurrentProject() const
{
    return currentProject;
}
