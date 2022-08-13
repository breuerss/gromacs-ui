#ifndef QUEUE_H
#define QUEUE_H

#include <QObject>
#include <deque>
#include <memory>
#include <utility>

namespace Command {

class Executor;

class Queue : public QObject
{
  Q_OBJECT
public:
  explicit Queue(QObject *parent = nullptr);

  Queue* clear();
  Queue* enqueue(std::shared_ptr<Executor> executor, bool needsPrevious = true);
  Queue* remove(size_t at);
  Queue* insert(size_t at, std::shared_ptr<Executor> executor, bool needsPrevious = true);
  std::shared_ptr<Executor> first();
  std::shared_ptr<Executor> last();
  void start();
  void stop();
  bool wasSuccessful() const;
  bool isRunning() const;
  size_t getSize() const;

signals:
  void finished(bool success);
  void stepFinished(size_t stepIndex, std::shared_ptr<Executor> step, bool success);
  void stepStarted(size_t stepIndex, std::shared_ptr<Executor> step);

private:
  void execNext();
  void disconnectCurrent();
  void updateLink(std::shared_ptr<Executor> prevLink, std::shared_ptr<Executor> link);
  QMetaObject::Connection conn;
  bool failed;
  size_t numberOfSteps;
  bool running = false;

  std::deque<std::pair<std::shared_ptr<Executor>, bool>> queue;
  std::shared_ptr<Executor> current;
};

}

#endif // QUEUE_H
