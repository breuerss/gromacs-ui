#include "step.h"
#include "../uiupdater.h"
#include <QObject>
#include <QDebug>
#include "../../ui/pdbcode.h"
#include "src/command/filenamegenerator.h"
#include "src/command/fileobjectconsumer.h"

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

void Step::showStatusUI(bool show)
{
  QWidget* widget = nullptr;
  if (show)
  {
    widget = command->getStatusUi();
  }
  UiUpdater::getInstance()->showStatusUI(widget);
}

}

