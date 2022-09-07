#ifndef PIPELINE_STEP_H
#define PIPELINE_STEP_H

#include "../command/fileobjectconsumer.h"
#include "../command/fileobjectprovider.h"
#include "../config/configuration.h"
#include "../command/executor.h"
#include "src/command/filenamegenerator.h"
#include <QString>
#include <memory>

namespace Model {
class Project;
}

namespace Pipeline {

class Step {
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
  virtual QString getType() const = 0;
  typedef std::shared_ptr<Step> Pointer;
  virtual Pointer create(std::shared_ptr<Model::Project>) const = 0;
  virtual ~Step() = default;

  const std::shared_ptr<Command::FileObjectConsumer>& getFileObjectConsumer() const;
  std::shared_ptr<Command::FileObjectConsumer>& getFileObjectConsumer();
  const std::shared_ptr<Command::FileObjectProvider>& getFileObjectProvider();
  const std::shared_ptr<Config::Configuration>& getConfiguration();
  const std::shared_ptr<Command::Executor>& getCommand();
  const std::shared_ptr<Command::FileNameGenerator>& getFileNameGenerator();

  void showStatusUI(bool show = true);

  const Category category;

protected:
  std::shared_ptr<Command::FileObjectConsumer> fileObjectConsumer;
  std::shared_ptr<Command::FileObjectProvider> fileObjectProvider;
  std::shared_ptr<Config::Configuration> configuration;
  std::shared_ptr<Command::Executor> command;
  std::shared_ptr<Command::FileNameGenerator> fileNameGenerator;
};

QDataStream &operator<<(QDataStream &out, const Step::Pointer step);
QDataStream &operator>>(QDataStream &in, Step::Pointer step);
}

#endif
