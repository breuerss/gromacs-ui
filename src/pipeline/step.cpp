#include "step.h"
#include "../uiupdater.h"
#include "../../ui/pdbcode.h"
#include "src/command/filenamegenerator.h"
#include "src/command/fileobjectconsumer.h"
#include "src/command/fileobjectprovider.h"
#include "src/command/executor.h"
#include "../model/project.h"

#include <QObject>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

namespace Pipeline {

Step::Step(
  std::shared_ptr<Model::Project> newProject,
  const QMap<Command::FileObject::Category, QList<Command::FileObject::Type>>& requiresMap,
  const QList<Command::FileObject::Type> providesList,
  std::shared_ptr<Config::Configuration> newConfiguration,
  std::shared_ptr<Command::Executor> newCommand,
  std::shared_ptr<Command::FileNameGenerator> newFileNameGenerator,
  Category newCategory
  )
  : category(newCategory)
  , project(newProject)
  , fileObjectConsumer(std::make_shared<Command::FileObjectConsumer>(requiresMap))
  , fileObjectProvider(std::make_shared<Command::FileObjectProvider>(providesList))
  , configuration(newConfiguration)
  , command(newCommand)
  , fileNameGenerator(newFileNameGenerator)
{
  command->setConfig(configuration.get());
  command->setFileObjectConsumer(fileObjectConsumer.get());
  command->setFileNameGenerator(fileNameGenerator.get());

  if (fileNameGenerator)
  {
    fileNameGenerator->setConfiguration(configuration);
    fileNameGenerator->setFileObjectConsumer(fileObjectConsumer);
  }

  auto updateFileNames = [this] {
    if (fileNameGenerator)
    {
      for (auto fileObject: fileObjectProvider->provides())
      {
        QString fileName = fileNameGenerator->getFileNameFor(fileObject->type);
        fileName = fileName.split("/", Qt::SkipEmptyParts).join("/");
        if (!fileName.isEmpty())
        {
          fileName = "/" + fileName;
        }
        fileObject->setFileName(fileName);
      }
    }
  };

  if (configuration)
  {
    conns << QObject::connect(
      configuration.get(), &Config::Configuration::anyChanged,
      updateFileNames);
  }
  conns << QObject::connect(
    project.get(), &Model::Project::nameChanged, updateFileNames);
  conns << QObject::connect(
    fileObjectConsumer.get(), &Command::FileObjectConsumer::connectedToChanged,
    [this, updateFileNames] (auto newFileObject, auto, auto oldFileObject)
    {
      if (newFileObject)
      {
        fileChangeConns[newFileObject] = QObject::connect(
          newFileObject.get(), &Command::FileObject::fileNameChanged, updateFileNames);
      }

      if (oldFileObject && fileChangeConns.contains(oldFileObject))
      {
        QObject::disconnect(fileChangeConns[oldFileObject]);
      }
      updateFileNames();
    }
    );
  updateFileNames();
}

Step::~Step()
{
  for (auto conn: conns)
  {
    QObject::disconnect(conn);
  }
  for (auto conn: fileChangeConns.values())
  {
    QObject::disconnect(conn);
  }
}

std::shared_ptr<Command::FileObjectConsumer>
Step::getFileObjectConsumer() const
{
  return fileObjectConsumer;
}

std::shared_ptr<Command::FileObjectProvider>
Step::getFileObjectProvider() const
{
  return fileObjectProvider;
}

const std::shared_ptr<Config::Configuration>
Step::getConfiguration() const
{
  return configuration;
}

const std::shared_ptr<Command::Executor>
Step::getCommand() const
{
  return command;
}

const std::shared_ptr<Command::FileNameGenerator>
Step::getFileNameGenerator() const
{
  return fileNameGenerator;
}

void Step::showStatusUI(bool show)
{
  QWidget* widget = nullptr;
  if (show)
  {
    widget = command->getStatusUi();
  }
  UiUpdater::getInstance()->showStatusUI(widget);
}

QJsonObject &operator<<(QJsonObject &out, const Step::Pointer step)
{
  if (step->getConfiguration())
  {
    out << step->getConfiguration();
  }
  const auto& location = step->location;
  out["type"] = step->getType();
  out["location"] = QJsonArray({
    location.x(), location.y(), location.width(), location.height()
  });

  QJsonArray fileObjectsArray;
  const auto& fileObjects = step->getFileObjectProvider()->provides();
  for (const auto& fileObject : fileObjects)
  {
    QJsonObject fileObjectJson;
    fileObjectJson << fileObject;
    fileObjectsArray.append(fileObjectJson);
  }

  out["provided-files"] = fileObjectsArray;

  return out;
}

QJsonObject &operator>>(QJsonObject &in, Step::Pointer step)
{
  if (step->getConfiguration())
  {
    in >> step->getConfiguration();
  }
  if (in.contains("location") && in["location"].isArray())
  {
    auto& location = step->location;
    QJsonArray jsonLoc = in["location"].toArray();
    location.setTopLeft({ jsonLoc[0].toDouble(), jsonLoc[1].toDouble() });
    location.setSize({ jsonLoc[2].toDouble(), jsonLoc[3].toDouble() });
  }

  if (in.contains("provided-files") && in["provided-files"].isArray())
  {
    QJsonArray files = in["provided-files"].toArray();
    const auto& provides = step->getFileObjectProvider()->provides();
    for (int index = 0; index < provides.size(); index++)
    {
      QJsonObject fileObjectJson = files[index].toObject();
      fileObjectJson >> provides[index];
    }
  }

  return in;
}

void Step::showUI(bool show)
{
  QWidget* widget = nullptr;
  if (show && configuration)
  {
    widget = configuration->getUI();
  }
  UiUpdater::getInstance()->showConfigUI(widget);
}


}
