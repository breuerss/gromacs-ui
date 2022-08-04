#include "project.h"
#include "systemsetup.h"
#include "settings.h"

Project::Project(const QString& newName)
    : systemSetup(new SystemSetup(this))
    , name(newName)
{

}

void Project::addStep()
{
    steps.push_back(std::make_shared<Step>());
}

const std::vector<std::shared_ptr<Step>>& Project::getSteps() const
{
    return steps;
}

std::shared_ptr<SystemSetup> Project::getSystemSetup()
{
    return systemSetup;
}

const QString& Project::getName()
{
    return name;
}

QString Project::getProjectPath()
{
    Settings settings;
    auto projectsPath = settings.value(Settings::PROJECTS_DIRECTORY).toString();
    return projectsPath + "/" + name;
}
