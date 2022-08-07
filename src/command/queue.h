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
    void start();
    bool wasSuccessful();

signals:
    void finished(bool success);
    void stepFinished(int stepIndex, bool success);

private:
    void execNext();
    bool failed;
    int numberOfSteps;

    std::deque<std::pair<std::shared_ptr<Executor>, bool>> queue;
    std::shared_ptr<Executor> current;
};

}

#endif // QUEUE_H
