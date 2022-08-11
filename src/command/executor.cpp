#include "executor.h"
#include "../statusmessagesetter.h"
#include <QDebug>
#include "../logforwarder.h"

namespace Command {

Executor::Executor(QObject *parent)
  : QObject{parent}
, mHasRun(false)
{
  connect(
    &process,
    QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
    [this] (int, QProcess::ExitStatus) {
      mHasRun = true;
      running = false;
      QString command = process.program() + " " + process.arguments().join(" ");
      QString message("Error executing " + command);
      if (process.exitCode() == 0)
      {
        message = "Sucessfully executed " + command;
      }

      StatusMessageSetter::getInstance()->setMessage(message);
      emit finished();
    });
  LogForwarder::getInstance()->listenTo(&process);
}

Executor::~Executor()
{
  LogForwarder::getInstance()->detach(&process);
}

void Executor::exec()
{
  running = true;
  emit started();
  doExecute();
}

void Executor::stop()
{
  qDebug() << "stopping";
  process.terminate();
  process.waitForFinished();
}

bool Executor::hasRun() const
{
  return mHasRun;
}

bool Executor::isRunning() const
{
  return running;
}

bool Executor::wasSuccessful() const
{
  return hasRun() &&
    process.exitStatus() == QProcess::NormalExit &&
    process.exitCode() == 0;
}

}
