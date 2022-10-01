#include "executor.h"
#include "../statusmessagesetter.h"
#include "../logforwarder.h"
#include "fileobjectconsumer.h"
#include <QDebug>
#include <memory>

namespace Command {

Executor::Executor()
  : QObject(nullptr)
    , mHasRun(false)
{
  conns << connect(
    &process,
    QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
    [this] () {
      mHasRun = true;
      setRunning(false);
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
      setRunning(true);
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
  if (!canExecute())
  {
    return;
  }
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

void Executor::setConfig(Config::Configuration* newConfig)
{
  configuration = newConfig;
  configChanged(configuration);
}

void Executor::setFileObjectConsumer(
  const Command::FileObjectConsumer* newFileObjectConsumer)
{
  for (auto conn: fileConsumerConnections)
  {
    disconnect(conn);
  }
  fileObjectConsumer = newFileObjectConsumer;

  fileConsumerConnections << connect(
    fileObjectConsumer, &FileObjectConsumer::connectedToChanged,
    [this] (
      FileObject::Pointer newFileObject,
      InputOutput::Category,
      FileObject::Pointer oldFileObject) {
      if (fileObjectConnections.contains(oldFileObject.get()))
      {
        disconnect(fileObjectConnections[oldFileObject.get()]);
      }

      if (newFileObject)
      {
        fileObjectConnections[newFileObject.get()] = connect(
          newFileObject.get(), &FileObject::fileNameChanged,
          [this] () {
            canExecuteChanged(canExecute());
          });
      }
      canExecuteChanged(canExecute());
  });
}

void Executor::setRunning(bool newRunning)
{
  running = newRunning;
  runningChanged(running);
}

void Executor::setFileNameGenerator(const FileNameGenerator* newFileNameGenerator)
{
  fileNameGenerator = newFileNameGenerator;
}

}
