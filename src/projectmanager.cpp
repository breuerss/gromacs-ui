#include "projectmanager.h"
#include "model/project.h"

#include <memory>
#include <exception>
#include <QFileDialog>
#include <QMessageBox>
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
  QMessageBox::StandardButton choice = QMessageBox::Yes;
  if (currentProject)
  {
    choice = QMessageBox::question(
      nullptr, tr("New Project"),
      tr("Do you really want to dismiss all changes in your current project?"));
  }
  if (choice == QMessageBox::Yes)
  {
    fileName.clear();
    currentProject.reset(new Model::Project());
    emit currentProjectChanged(currentProject);
  }
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
    QString defaultSuffix(".groproj");
    QString filter = "*" + defaultSuffix;

    writeToFileName = QFileDialog::getSaveFileName(
      nullptr,
      tr("Save Gromacs Project File"),
      QDir::homePath(),
      filter
      );

    if (!writeToFileName.endsWith(defaultSuffix))
    {
      writeToFileName += defaultSuffix;
    }
  }

  if (QFile(writeToFileName).exists())
  {
    QMessageBox::StandardButton choice = QMessageBox::question(
      nullptr,
      tr("File exits"),
      tr("File '%1' already exists. Do you want to overwrite?").arg(writeToFileName));
    if (choice == QMessageBox::No)
    {
      saveAs();
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
