#include "projectmanager.h"
#include "model/project.h"
#include "undoredo/stack.h"

#include <memory>
#include <exception>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

ProjectManager* ProjectManager::getInstance()
{
  static ProjectManager instance;
  static bool initiated = false;
  if (!initiated)
  {
    instance.createNewProject();
    initiated = true;
  }

  return &instance;
}

ProjectManager::ProjectManager()
  : currentProject(std::make_shared<Model::Project>())
{
}

std::shared_ptr<Model::Project> ProjectManager::getCurrentProject() const
{
  return currentProject;
}

const QString& ProjectManager::getFileName() const
{
  return fileName;
}

void ProjectManager::createNewProject()
{
  QMessageBox::StandardButton choice = QMessageBox::Yes;
  if (currentProject && !UndoRedo::Stack::getInstance()->isClean())
  {
    choice = QMessageBox::question(
      nullptr, tr("New Project"),
      tr("Do you really want to dismiss all changes in your current project?"));
  }

  if (choice == QMessageBox::Yes)
  {
    fileName.clear();
    currentProject->clearSteps();
    currentProject->setProperty("name", "");
    UndoRedo::Stack::getInstance()->clear();
    emit currentProjectChanged(currentProject);
  }
}

void ProjectManager::save(const QString& saveToFileName)
{
  QString writeToFileName = saveToFileName;
  if (writeToFileName.isEmpty())
  {
    writeToFileName = fileName;
  }

  if (writeToFileName.isEmpty())
  {
    saveAs();
  }
  QFile file(writeToFileName);
  file.open(QFile::WriteOnly);
  QJsonObject data;
  data << currentProject;
  file.write(QJsonDocument(data).toJson());
  file.close();
  UndoRedo::Stack::getInstance()->setClean();
}

void ProjectManager::saveAs()
{
  saveAs("");
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
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
      filter
      );

    if (!writeToFileName.isEmpty() && !writeToFileName.endsWith(defaultSuffix))
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

  save(writeToFileName);
  if (!writeToFileName.isEmpty() && fileName.isEmpty())
  {
    fileName = writeToFileName;
  }
}

void ProjectManager::open(const QString& newFileName)
{
  QString fileNameToOpen = newFileName;

  if (fileNameToOpen.isEmpty())
  {
    fileNameToOpen = QFileDialog::getOpenFileName(
      nullptr,
      tr("Open Gromacs Simulation Project"),
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
      "*.groproj"
      );
  }

  QFile file(fileNameToOpen);
  if (file.exists() && file.open(QFile::ReadOnly))
  {
    QByteArray saveData = file.readAll();
    file.close();

    if (!currentProject)
    {
      createNewProject();
    }

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject obj = loadDoc.object();
    obj >> currentProject;
    fileName = fileNameToOpen;
    currentProjectChanged(currentProject);
  }

  UndoRedo::Stack::getInstance()->clear();
}
