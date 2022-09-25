#ifndef PIPELINE_STEP_H
#define PIPELINE_STEP_H

#include "../config/configuration.h"
#include "../command/fileobject.h"
#include <QString>
#include <memory>

namespace config {
class Configuration;
}

namespace Model {
class Project;
}

namespace Command {
class FileObjectProvider;
class FileObjectConsumer;
class FileNameGenerator;
class Executor;
}

namespace Pipeline {

class Step {
public:
  enum class Category {
    Unknown,
    DataProvider,
    PreProcess,
    PostProcess,
    Simulation,
    Analysis,
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
  virtual ~Step() = default;

  std::shared_ptr<Command::FileObjectConsumer> getFileObjectConsumer() const;
  std::shared_ptr<Command::FileObjectProvider> getFileObjectProvider() const;

  const std::shared_ptr<Config::Configuration> getConfiguration() const;
  const std::shared_ptr<Command::Executor> getCommand() const;
  const std::shared_ptr<Command::FileNameGenerator> getFileNameGenerator() const;

  void showStatusUI(bool show = true);
  void showUI(bool show = true);

  const Category category;
  QRectF location;

protected:
  std::shared_ptr<Command::FileObjectConsumer> fileObjectConsumer;
  std::shared_ptr<Command::FileObjectProvider> fileObjectProvider;
  std::shared_ptr<Config::Configuration> configuration;
  std::shared_ptr<Command::Executor> command;
  std::shared_ptr<Command::FileNameGenerator> fileNameGenerator;
};

QJsonObject &operator<<(QJsonObject &out, const Step::Pointer step);
QJsonObject &operator>>(QJsonObject &in, Step::Pointer step);
}

#endif
