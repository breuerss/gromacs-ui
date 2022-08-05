#include "projectmanager.h"

#include <memory>
#include <exception>
#include <QFileDialog>
#include <QDebug>

ProjectManager* ProjectManager::getInstance()
{
    static std::unique_ptr<ProjectManager> instance;
    if (!instance)
    {
        instance.reset(new ProjectManager());
        instance->createNewProject();
    }

    return instance.get();
}

void ProjectManager::addStep()
{
    qDebug() << "ProjectManager::addStep";
    if (currentProject)
    {
        currentProject->addStep();
        // TODO adding a step should result in UI update instead
        //of emitting that the whole project changed
        emit currentProjectChanged(currentProject);
    }
}

const std::shared_ptr<Project> ProjectManager::getCurrentProject() const
{
    return currentProject;
}

void ProjectManager::createNewProject()
{
    // TODO get name from input mask
    currentProject.reset(new Project("test"));
    emit currentProjectChanged(currentProject);
}

void ProjectManager::save()
{
    if (fileName.isEmpty())
    {
        saveAs();
    }
    else
    {
        saveAs(fileName);
    }
}

void ProjectManager::saveAs(const QString& saveAsFileName)
{
    QString writeToFileName = saveAsFileName;
    if (writeToFileName.isEmpty())
    {
        writeToFileName = QFileDialog::getSaveFileName(
                    nullptr,
                    tr("Save Gromacs Project File"),
                    QDir::homePath(),
                    "*.groproj"
                    );
    }

    if (!writeToFileName.isEmpty())
    {
        QFile file(writeToFileName);
        file.open(QFile::WriteOnly);
        QDataStream out(&file);
        out << *(currentProject.get());
        file.close();

        if (fileName.isEmpty())
        {
            fileName = writeToFileName;
        }
    }
}

void ProjectManager::open()
{
    fileName = QFileDialog::getOpenFileName(
                nullptr,
                tr("Open Gromacs Simulation Project"),
                QDir::homePath(),
                "*.groproj"
                );
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        file.open(QFile::ReadOnly);
        QDataStream data(&file);
        if (!currentProject)
        {
            createNewProject();
        }
        data >> *(currentProject.get());
        file.close();
        currentProjectChanged(currentProject);
    }
}
