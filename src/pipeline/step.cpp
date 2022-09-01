#include "step.h"
#include "../uiupdater.h"
#include <memory>
#include <QObject>
#include <variant>
#include <QDebug>
#include "../form/simulationsetupform.h"
#include "../../ui/simulationstatus.h"
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
  command->setConfig(configuration);
  command->setFileObjectProvider(fileObjectProvider);

  QObject::connect(command.get(), &Command::Executor::finished, [this] {
    for (auto fileObject: fileObjectProvider->provides())
    {
      fileObject->setFileName(fileNameGenerator->getFileNameFor(fileObject->type));
    }
  });
}

void Step::showConfigUI(bool show)
{
  QWidget* widget = nullptr;
  if (show)
  {
    widget = configuration->getUI();
  }
  UiUpdater::getInstance()->showConfigUI(widget);
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

