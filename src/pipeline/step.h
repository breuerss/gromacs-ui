#ifndef PIPELINE_STEP_H
#define PIPELINE_STEP_H

#include "../command/types.h"
#include <QString>
#include <QMap>
#include <QList>
#include <QPointF>
#include <QObject>
#include <memory>

namespace Config {
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

class Step: public QObject {
  Q_OBJECT
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
    std::shared_ptr<Model::Project> newProject,
    const QMap<Command::InputOutput::Category, QList<Command::FileObject::Type>>& requiresMap,
    const QList<Command::FileObject::Type> providesList,
    std::shared_ptr<Config::Configuration> configuration,
    std::shared_ptr<Command::Executor> command,
    std::shared_ptr<Command::FileNameGenerator> fileNameGenerator
    );

  explicit Step(
    std::shared_ptr<Model::Project> newProject,
    const QMap<Command::InputOutput::Category, QList<Command::FileObject::Type>>& requiresMap,
    const QList<Command::Data> providesList,
    std::shared_ptr<Config::Configuration> configuration,
    std::shared_ptr<Command::Executor> command,
    std::shared_ptr<Command::FileNameGenerator> fileNameGenerator
    );
  virtual QString getName() const = 0;
  virtual QString getType() const = 0;
  virtual Category getCategory() const = 0;
  typedef std::shared_ptr<Step> Pointer;
  virtual ~Step();

  std::shared_ptr<Command::FileObjectConsumer> getFileObjectConsumer() const;
  std::shared_ptr<Command::FileObjectProvider> getFileObjectProvider() const;

  const std::shared_ptr<Config::Configuration> getConfiguration() const;
  const std::shared_ptr<Command::Executor> getCommand() const;
  const std::shared_ptr<Command::FileNameGenerator> getFileNameGenerator() const;

  void showStatusUI(bool show = true);
  void showUI(bool show = true);

  void setLocation(const QPointF& newLocation);
  const QPointF& getLocation() const;

signals:
  void locationChanged(const QPointF& location);

protected:
  std::shared_ptr<Model::Project> project;
  std::shared_ptr<Command::FileObjectConsumer> fileObjectConsumer;
  std::shared_ptr<Command::FileObjectProvider> fileObjectProvider;
  std::shared_ptr<Config::Configuration> configuration;
  std::shared_ptr<Command::Executor> command;
  std::shared_ptr<Command::FileNameGenerator> fileNameGenerator;

  QList<QMetaObject::Connection> conns;
  QMap<std::shared_ptr<Command::FileObject>, QMetaObject::Connection> fileChangeConns;
  QPointF location;
};

QJsonObject &operator<<(QJsonObject &out, const Step::Pointer step);
QJsonObject &operator>>(QJsonObject &in, Step::Pointer step);
}

#endif
