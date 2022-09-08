#include "step.h"
#include "../uiupdater.h"
#include "../../ui/pdbcode.h"
#include "src/command/filenamegenerator.h"
#include "src/command/fileobjectconsumer.h"

#include <QObject>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

namespace Pipeline {

Step::Step(
  const QMap<Command::FileObject::Category, QList<Command::FileObject::Type>>& requiresMap,
  const QList<Command::FileObject::Type> providesList,
  std::shared_ptr<Config::Configuration> newConfiguration,
  std::shared_ptr<Command::Executor> newCommand,
  std::shared_ptr<Command::FileNameGenerator> newFileNameGenerator,
  Category newCategory
  )
  : category(newCategory)
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
  }

  auto updateFileNames = [this] {
    if (fileNameGenerator)
    {
      for (auto fileObject: fileObjectProvider->provides())
      {
        fileObject->setFileName(fileNameGenerator->getFileNameFor(fileObject->type));
      }
    }
  };

  QObject::connect(command.get(), &Command::Executor::finished, updateFileNames);
  updateFileNames();
}

const std::shared_ptr<Command::FileObjectConsumer>&
Step::getFileObjectConsumer() const
{
  return fileObjectConsumer;
}

std::shared_ptr<Command::FileObjectConsumer>&
Step::getFileObjectConsumer()
{
  return fileObjectConsumer;
}

const std::shared_ptr<Command::FileObjectProvider>&
Step::getFileObjectProvider() const
{
  return fileObjectProvider;
}

std::shared_ptr<Command::FileObjectProvider>&
Step::getFileObjectProvider()
{
  return fileObjectProvider;
}

const std::shared_ptr<Config::Configuration>&
Step::getConfiguration()
{
  return configuration;
}

const std::shared_ptr<Command::Executor>&
Step::getCommand()
{
  return command;
}

const std::shared_ptr<Command::FileNameGenerator>&
Step::getFileNameGenerator()
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

QDataStream &operator<<(QDataStream &out, const Step::Pointer step)
{
  out << step->getConfiguration();
  out << step->location;

  const auto& fileObjects = step->getFileObjectProvider()->provides();
  for (const auto& fileObject : fileObjects)
  {
    out << fileObject;
  }

  return out;
}

QDataStream &operator>>(QDataStream &in, Step::Pointer step)
{
  in >> step->getConfiguration();
  in >> step->location;

  int noOfFileObjects;
  in >> noOfFileObjects;
  for (auto fileObject : step->getFileObjectProvider()->provides())
  {
    in >> fileObject;
  }

  return in;
}

QJsonObject &operator<<(QJsonObject &out, const Step::Pointer step)
{
  out << step->getConfiguration();
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
  in >> step->getConfiguration();
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

}
