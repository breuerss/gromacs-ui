#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QObject>
#include <QProcess>
#include <QString>

#include "src/config/configuration.h"

namespace Command {

class FileObjectProvider;

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
  virtual QString getName() const { return QString(); }
  virtual QWidget* getStatusUi() { return nullptr; }
  void exec();
  void stop();

  virtual bool canExecute() const = 0;
  bool hasRun() const;
  bool isRunning() const;
  bool wasSuccessful() const;

  void setConfig(std::shared_ptr<Config::Configuration> newConfig);
  void setFileObjectProvider(std::shared_ptr<Command::FileObjectProvider> fileObjectProvider);

signals:
  void finished();
  void started();
  void progress(float percentage, ProgressType type = ProgressType::Percentage);
  void runningChanged(bool running);
  void canExecuteChanged(bool canExecute);
  void configChanged(std::shared_ptr<Config::Configuration> newConfig);

protected:
  QProcess process;
  std::shared_ptr<Config::Configuration> configuration;
  std::shared_ptr<Command::FileObjectProvider> fileObjectProvider;
  void setRunning(bool running);

private:
  bool mHasRun = false;
  bool terminationRequested = false;
  bool running = false;
  QList<QMetaObject::Connection> conns;
};

}

#endif // EXECUTOR_H
