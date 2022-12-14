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

      const bool successful = process.exitCode() == 0;
      if (successful)
      {
        message = "Sucessfully executed ";
      }
      else if (terminationRequested)
      {
        message = "Terminated command ";
      }
      message += command;

      StatusMessageSetter::getInstance()->setMessage(message);
      finished(successful);
    });
  conns << connect(
    &process,
    &QProcess::errorOccurred,
    [this] (QProcess::ProcessError error) {
      if (error == QProcess::FailedToStart || error == QProcess::Crashed)
      {
        setRunning(false);
        finished(false);
      }
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
      const Command::Data& newData,
      InputOutput::Category,
      const Command::Data& oldData) {
      if (fileObjectConnections.contains(oldData))
      {
        disconnect(fileObjectConnections[oldData]);
      }

      if (isSet<FileObject::Pointer>(newData))
      {
        fileObjectConnections[newData] = connect(
          std::get<FileObject::Pointer>(newData).get(),
          &FileObject::fileNameChanged,
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
