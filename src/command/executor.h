#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QObject>
#include <QProcess>
#include <QString>

namespace Command {

class Executor : public QObject
{
  Q_OBJECT
public:
  enum class ProgressType {
    Percentage,
    Value,
  };
  explicit Executor(QObject *parent = nullptr);
  ~Executor();
  virtual void doExecute() = 0;
  virtual QString getName() const = 0;
  void exec();
  void stop();

  bool hasRun() const;
  bool isRunning() const;
  bool wasSuccessful() const;

signals:
  void finished();
  void started();
  void progress(float percentage, ProgressType type = ProgressType::Percentage);
  void runningChanged(bool running);

protected:
  QProcess process;

private:
  bool mHasRun = false;
  bool running = false;
  bool terminationRequested = false;
  QList<QMetaObject::Connection> conns;
};

}

#endif // EXECUTOR_H
