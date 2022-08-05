#include "project.h"
#include "systemsetup.h"
#include "../settings.h"
#include <QDebug>

Project::Project(const QString& newName)
    : systemSetup(new SystemSetup(this))
    , name(newName)
{
}

void Project::addStep()
{
    steps.push_back(std::make_shared<Step>());
}

void Project::removeStep(int index)
{
    steps.erase(steps.begin() + index);
}

const std::vector<std::shared_ptr<Step>>& Project::getSteps() const
{
    return steps;
}

std::shared_ptr<SystemSetup> Project::getSystemSetup() const
{
    return systemSetup;
}

const QString& Project::getName() const
{
    return name;
}

void Project::setName(const QString& newName)
{
    name = newName;
}

QString Project::getProjectPath()
{
    Settings settings;
    auto projectsPath = settings.value(Settings::PROJECTS_DIRECTORY).toString();
    return projectsPath + "/" + name;
}

QDataStream &operator<<(QDataStream &out, const Project &project)
{
    out << project.getName();
    out << *(project.getSystemSetup().get());

    int numberOfSteps = project.getSteps().size();
    out << numberOfSteps;

    for (auto step: project.getSteps())
    {
        out << *(step.get());
    }

    return out;
}

QDataStream &operator>>(QDataStream &in, Project &project)
{
    QString name;
    in >> name;
    project.setName(name);

    in >> *(project.getSystemSetup().get());

    int numberOfSteps;
    in >> numberOfSteps;

    qDebug() << numberOfSteps;
    for (int stepIndex = 0; stepIndex < numberOfSteps; ++stepIndex)
    {
       project.addStep();
    }

    for (auto step: project.getSteps())
    {
        qDebug() << "reading step";
        in >> *(step.get());
    }

    return in;
}
