#include "step.h"
#include "../uiupdater.h"
#include "../../ui/pdbcode.h"
#include "../command/filenamegenerator.h"
#include "../command/fileobjectconsumer.h"
#include "../command/fileobjectprovider.h"
#include "../command/executor.h"
#include "../command/types.h"
#include "../model/project.h"

#include <QObject>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

namespace Pipeline {

using FileObject = Command::FileObject;

Step::Step(
  std::shared_ptr<Model::Project> newProject,
  const QMap<Command::InputOutput::Category, QList<Command::FileObject::Type>>& requiresMap,
  const QList<Command::Data> providesList,
  std::shared_ptr<Config::Configuration> configuration,
  std::shared_ptr<Command::Executor> command,
  std::shared_ptr<Command::FileNameGenerator> fileNameGenerator
)
  : Step(
    newProject,
    requiresMap,
    QList<FileObject::Type>(),
    configuration,
    command,
    fileNameGenerator
    )
{
  fileObjectProvider = std::make_shared<Command::FileObjectProvider>(providesList);
}

Step::Step(
  std::shared_ptr<Model::Project> newProject,
  const QMap<Command::InputOutput::Category, QList<FileObject::Type>>& requiresMap,
  const QList<FileObject::Type> providesList,
  std::shared_ptr<Config::Configuration> newConfiguration,
  std::shared_ptr<Command::Executor> newCommand,
  std::shared_ptr<Command::FileNameGenerator> newFileNameGenerator
  )
  : project(newProject)
  , fileObjectConsumer(std::make_shared<Command::FileObjectConsumer>(requiresMap))
  , fileObjectProvider(std::make_shared<Command::FileObjectProvider>(providesList))
  , configuration(newConfiguration)
  , command(newCommand)
  , fileNameGenerator(newFileNameGenerator)
{
  if (fileNameGenerator)
  {
    fileNameGenerator->setConfiguration(configuration);
    fileNameGenerator->setFileObjectConsumer(fileObjectConsumer);
  }

  if (command)
  {
    command->setConfig(configuration.get());
    command->setFileObjectConsumer(fileObjectConsumer.get());
    command->setFileNameGenerator(fileNameGenerator.get());
    conns << QObject::connect(command.get(), &Command::Executor::finished, [this] () {
      for (auto step: project->getSteps())
      {
        if (step->getCommand())
        {
          step->getCommand()->canExecuteChanged(step->getCommand()->canExecute());
        }
      }
    });
  }

  if (fileNameGenerator)
  {
    using FileObject = Command::FileObject;
    auto updateFileNames = [this] {
      for (const auto& data: fileObjectProvider->provides())
      {
        if (Command::isSet<FileObject::Pointer>(data))
        {
          auto fileObject = std::get<FileObject::Pointer>(data);
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
      [this, updateFileNames] (
        const Command::Data& newData, const auto&, const Command::Data& oldData)
      {
        if (Command::isSet<FileObject::Pointer>(newData))
        {
          auto& fileObject = std::get<FileObject::Pointer>(newData);
          fileChangeConns[fileObject] = QObject::connect(
            fileObject.get(), &Command::FileObject::fileNameChanged, updateFileNames);
        }

        if (Command::isSet<FileObject::Pointer>(oldData))
        {
          auto& oldFileObject = std::get<FileObject::Pointer>(oldData);
          if (oldFileObject && fileChangeConns.contains(oldFileObject))
          {
            QObject::disconnect(fileChangeConns[oldFileObject]);
          }
          updateFileNames();
        }
      }
    );
    updateFileNames();
  }
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
  if (show && command)
  {
    widget = command->getStatusUi();
  }
  UiUpdater::getInstance()->showStatusUI(widget);
}

void Step::setLocation(const QPointF& newLocation)
{
  const bool changed = newLocation != location;

  location = newLocation;

  if (changed)
  {
    locationChanged(location);
  }
}

const QPointF& Step::getLocation() const
{
  return location;
}

QJsonObject &operator<<(QJsonObject &out, const Step::Pointer step)
{
  if (step->getConfiguration())
  {
    out << step->getConfiguration();
  }
  const auto& location = step->getLocation();
  out["type"] = step->getType();
  out["location"] = QJsonArray({
    location.x(), location.y()
  });

  QJsonArray fileObjectsArray;
  QJsonArray configObjectsArray;
  const auto& providedData = step->getFileObjectProvider()->provides();
  for (const auto& data : providedData)
  {
    QJsonObject jsonObject;

    std::visit([&jsonObject] (const auto& data) {
      jsonObject << data;
    }, data);
    if (std::holds_alternative<FileObject::Pointer>(data))
    {
      fileObjectsArray.append(jsonObject);
    }
    if (std::holds_alternative<Config::Configuration::Pointer>(data))
    {
      configObjectsArray.append(jsonObject);
    }
  }

  out["provided-files"] = fileObjectsArray;
  out["provided-configs"] = configObjectsArray;

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
    QJsonArray jsonLoc = in["location"].toArray();
    step->setLocation(QPointF(jsonLoc[0].toDouble(), jsonLoc[1].toDouble()));
  }

  if (in.contains("provided-files") && in["provided-files"].isArray())
  {
    QJsonArray files = in["provided-files"].toArray();
    const auto& provides = step->getFileObjectProvider()->provides();
    for (int index = 0; index < provides.size(); index++)
    {
      QJsonObject fileObjectJson = files[index].toObject();
      auto& data = provides[index];
      if (std::holds_alternative<FileObject::Pointer>(data))
      {
        auto& fileObject = std::get<FileObject::Pointer>(data);
        fileObjectJson >> fileObject;
      }

    }
  }

  if (in.contains("provided-configs") && in["provided-configs"].isArray())
  {
    QJsonArray files = in["provided-configs"].toArray();
    const auto& provides = step->getFileObjectProvider()->provides();
    for (int index = 0; index < provides.size(); index++)
    {
      QJsonObject fileObjectJson = files[index].toObject();
      auto& data = provides[index];
      if (std::holds_alternative<Config::Configuration::Pointer>(data))
      {
        auto& configObject = std::get<Config::Configuration::Pointer>(data);
        fileObjectJson >> configObject;
      }
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
