#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QObject>
#include <QProcess>

namespace Command {

class Executor : public QObject
{
  Q_OBJECT
public:
  explicit Executor(QObject *parent = nullptr);
  ~Executor();
  virtual void doExecute() = 0;
  void exec();
  void stop();

  bool hasRun() const;
  bool isRunning() const;
  bool wasSuccessful() const;

signals:
  void finished();
  void started();
  void progress(float percentage);
  void runningChanged(bool running);

protected:
  QProcess process;

private:
  bool mHasRun;
  bool running;
};

}

#endif // EXECUTOR_H
