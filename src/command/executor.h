#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QObject>
#include <QProcess>
#include <QString>

#include "fileobjectconsumer.h"
#include "src/config/configuration.h"
//#include "executorfileobjectprovider.h"

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
  //const FileObjectConsumer& getFileConsumer() const;
  //const ExecutorFileObjectProvider& getFileProvider() const;
  void setConfig(std::shared_ptr<Config::Configuration> newConfig);
  void setFileObjectProvider(std::shared_ptr<Command::FileObjectProvider> fileObjectProvider);

signals:
  void finished();
  void started();
  void progress(float percentage, ProgressType type = ProgressType::Percentage);
  void runningChanged(bool running);

protected:
  QProcess process;
  std::shared_ptr<Config::Configuration> configuration;
  std::shared_ptr<Command::FileObjectProvider> fileObjectProvider;
  //FileObjectConsumer fileConsumer;
  //ExecutorFileObjectProvider fileProvider;

private:
  bool mHasRun = false;
  bool running = false;
  bool terminationRequested = false;
  QList<QMetaObject::Connection> conns;
};

}

#endif // EXECUTOR_H
