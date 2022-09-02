#ifndef PIPELINE_STEP_H
#define PIPELINE_STEP_H

#include "../command/fileobjectconsumer.h"
#include "../command/fileobjectprovider.h"
#include "../config/configuration.h"
#include "../command/executor.h"
#include "src/command/filenamegenerator.h"
#include <QString>
#include <memory>

namespace Pipeline {

class Step : public std::enable_shared_from_this<Step> {
public:
  enum class Category {
    Unknown,
    DataProvider,
    Preprocess,
    Simulation,
    Viewer,
  };
  Step(
    const QMap<Command::FileObject::Category, QList<Command::FileObject::Type>>& requiresMap,
    const QList<Command::FileObject::Type> providesList,
    std::shared_ptr<Config::Configuration> configuration,
    std::shared_ptr<Command::Executor> command,
    std::shared_ptr<Command::FileNameGenerator> fileNameGenerator,
    Category category
    );
  virtual QString getName() const = 0;
  virtual ~Step() = default;

  const std::shared_ptr<Command::FileObjectConsumer>& getFileObjectConsumer() const { return fileObjectConsumer; }
  std::shared_ptr<Command::FileObjectConsumer>& getFileObjectConsumer() { return fileObjectConsumer; }
  const std::shared_ptr<Command::FileObjectProvider>& getFileObjectProvider() { return fileObjectProvider; }
  const std::shared_ptr<Config::Configuration>& getConfiguration() { return configuration; }
  const std::shared_ptr<Command::Executor>& getCommand() { return command; }
  const std::shared_ptr<Command::FileNameGenerator>& getFileNameGenerator() { return fileNameGenerator; }

  void showStatusUI(bool show = true);

  const Category category;

protected:
  std::shared_ptr<Command::FileObjectConsumer> fileObjectConsumer;
  std::shared_ptr<Command::FileObjectProvider> fileObjectProvider;
  std::shared_ptr<Config::Configuration> configuration;
  std::shared_ptr<Command::Executor> command;
  std::shared_ptr<Command::FileNameGenerator> fileNameGenerator;
};

}

#endif
