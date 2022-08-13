#include "queue.h"

#include <memory>
#include <QDebug>
#include "../logforwarder.h"
#include "executor.h"
#include "inputoutputfilelink.h"

namespace Command {

Queue::Queue(QObject *parent)
  : QObject(parent)
    , failed(false)
{
}

Queue* Queue::clear()
{
  queue.clear();
  disconnectCurrent();

  if (current)
  {
    current->stop();
    current.reset();
  }
  return this;
}

Queue* Queue::enqueue(std::shared_ptr<Executor> executor, bool needsPrevious)
{
  size_t beforeSize = queue.size();
  if (beforeSize > 0)
  {
    updateLink(queue.at(beforeSize - 1).first, executor);
  }
  queue.push_back({executor, needsPrevious});
  numberOfSteps = queue.size();

  return this;
}

void Queue::updateLink(std::shared_ptr<Executor> prevExecutor, std::shared_ptr<Executor> executor)
{
  auto prevLink = std::dynamic_pointer_cast<InputOutputFileLink>(prevExecutor);
  auto link = std::dynamic_pointer_cast<InputOutputFileLink>(executor);
  if (link && prevLink)
  {
    link->setPreviousLink(prevLink);
  }
}

Queue* Queue::remove(size_t at)
{
  queue.erase(queue.begin() + at);
  numberOfSteps = queue.size();

  return this;
}

Queue* Queue::insert(size_t at, std::shared_ptr<Executor> executor, bool needsPrevious)
{
  if (at > 0)
  {
    updateLink(queue.at(0).first, executor);
  }

  size_t beforeSize = queue.size();
  if (at != beforeSize)
  {
    updateLink(queue.at(at).first, executor);
  }

  queue.insert(queue.begin() + at, {executor, needsPrevious});
  numberOfSteps = queue.size();

  return this;
}

void Queue::start()
{
  qDebug() << __PRETTY_FUNCTION__;
  execNext();
}

std::shared_ptr<Executor> Queue::first()
{
  return queue.front().first;
}

std::shared_ptr<Executor> Queue::last()
{
  return queue.back().first;
}

bool Queue::wasSuccessful() const
{
  return !failed;
}

void Queue::execNext()
{
  qDebug() << __PRETTY_FUNCTION__;
  if (queue.size() > 0)
  {
    running = true;
    qDebug() << "queue has elements" << queue.size();
    auto nextCommand = queue.front();
    bool needsPrevious = nextCommand.second;

    if (
      !needsPrevious ||
      (needsPrevious && (!current || (current && current->wasSuccessful()))))
    {
      qDebug() << "can run";
      current = nextCommand.first;
      conn = connect(current.get(), &Executor::finished, [this] () {
        disconnectCurrent();
        emit stepFinished(numberOfSteps - queue.size() - 1, current, current->wasSuccessful());
        running = false;
        execNext();
      });
      queue.pop_front();
      emit stepStarted(numberOfSteps - queue.size() - 1, current);
      current->exec();
    }
    else
    {
      failed = true;
      emit finished(!failed);
    }
  }
  else
  {
    running = false;
    emit finished(!failed);
  }
}

size_t Queue::getSize() const
{
  return numberOfSteps;
}

bool Queue::isRunning() const
{
  return running;
}

void Queue::stop()
{
  disconnectCurrent();
  if (current)
  {
    current->stop();
  }
  running = false;
}

void Queue::disconnectCurrent()
{
  if (current)
  {
    disconnect(conn);
  }
}

}
