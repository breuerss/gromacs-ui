#include "executor.h"
#include "../statusmessagesetter.h"
#include "../logforwarder.h"
#include <QDebug>

namespace Command {

Executor::Executor(QObject *parent)
  : QObject{parent}
, mHasRun(false)
{
  conns << connect(
    &process,
    QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
    [this] (int, QProcess::ExitStatus) {
      mHasRun = true;
      running = false;
      emit runningChanged(running);
      QString command = process.program() + " " + process.arguments().join(" ");
      QString message("Error executing ");
      if (process.exitCode() == 0)
      {
        message = "Sucessfully executed ";
      }
      else if (terminationRequested)
      {
        message = "Terminated command ";
      }
      message += command;

      StatusMessageSetter::getInstance()->setMessage(message);
      emit finished();
    });
  conns << connect(
    &process,
    &QProcess::started,
    [this] () {
      running = true;
    });
  LogForwarder::getInstance()->listenTo(&process);
}

Executor::~Executor()
{
  LogForwarder::getInstance()->detach(&process);
  for (auto& conn : conns)
  {
    disconnect(conn);
  }
  process.kill();
  process.waitForFinished();
}

void Executor::exec()
{
  terminationRequested = false;
  emit runningChanged(running);
  emit started();
  doExecute();
}

void Executor::stop()
{
  qDebug() << "stopping";
  terminationRequested = true;
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
