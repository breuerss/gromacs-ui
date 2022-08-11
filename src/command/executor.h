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
  bool wasSuccessful() const;

signals:
  void finished();
  void started();

protected:
  QProcess process;

private:
  bool mHasRun;

};

}

#endif // EXECUTOR_H
