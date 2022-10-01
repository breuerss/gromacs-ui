#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "types.h"

#include <QObject>
#include <QProcess>
#include <QString>
#include <QMap>
#include <memory>

namespace Config {
class Configuration;
}

namespace Command {

class FileObjectConsumer;
class FileNameGenerator;
class FileObject;

class Executor : public QObject
{
  Q_OBJECT
public:
  enum class ProgressType {
    Percentage,
    Value,
  };
  Executor();
  ~Executor();
  virtual void doExecute() = 0;
  virtual QWidget* getStatusUi() { return nullptr; }
  void exec();
  void stop();

  virtual bool canExecute() const = 0;
  bool hasRun() const;
  bool isRunning() const;
  bool wasSuccessful() const;

  void setConfig(Config::Configuration* newConfig);
  void setFileObjectConsumer(const Command::FileObjectConsumer* fileObjectConsumer);
  void setFileNameGenerator(const FileNameGenerator* newfileNameGenerator);

signals:
  void finished();
  void started();
  void progress(float percentage, ProgressType type = ProgressType::Percentage);
  void runningChanged(bool running);
  void canExecuteChanged(bool canExecute);
  void configChanged(const Config::Configuration* newConfig);

protected:
  QProcess process;
  Config::Configuration* configuration;
  const Command::FileObjectConsumer* fileObjectConsumer;
  const FileNameGenerator* fileNameGenerator;
  void setRunning(bool running);

private:
  bool mHasRun = false;
  bool terminationRequested = false;
  bool running = false;
  QList<QMetaObject::Connection> conns;
  QList<QMetaObject::Connection> fileConsumerConnections;
  QMap<Command::Data, QMetaObject::Connection> fileObjectConnections;
};

}

#endif // EXECUTOR_H
