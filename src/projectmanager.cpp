#include "projectmanager.h"
#include "model/project.h"

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

const std::shared_ptr<Model::Project> ProjectManager::getCurrentProject() const
{
    return currentProject;
}

void ProjectManager::createNewProject()
{
    // TODO get name from input mask
    currentProject.reset(new Model::Project("test"));
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
        QString suffix(".groproj");
        writeToFileName = QFileDialog::getSaveFileName(
                    nullptr,
                    tr("Save Gromacs Project File"),
                    QDir::homePath(),
                    "*" + suffix
                    );

        if (!writeToFileName.endsWith(suffix))
        {
          writeToFileName += suffix;
        }
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
