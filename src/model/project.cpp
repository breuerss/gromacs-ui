#include "project.h"
#include "systemsetup.h"
#include "step.h"
#include "../settings.h"
#include <QDebug>

Project::Project(const QString& newName)
    : systemSetup(new SystemSetup(this))
    , name(newName)
{
}

std::shared_ptr<Step> Project::addStep()
{
    auto step = std::make_shared<Step>();
    steps.push_back(step);
    emit stepAdded(step, steps.size());
    return step;
}

void Project::removeStep(int at)
{
    auto step = steps[at];
    steps.erase(steps.begin() + at);
    emit stepRemoved(step, at);
}

void Project::clearSteps()
{
    while (steps.size())
    {
        removeStep(0);
    }
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

    while (project.getSteps().size())
    {
      project.removeStep(project.getSteps().size() - 1);
    }

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
