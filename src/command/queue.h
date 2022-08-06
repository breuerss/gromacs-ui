#ifndef QUEUE_H
#define QUEUE_H

#include "executor.h"

#include <QObject>
#include <deque>
#include <memory>
#include <utility>

namespace Command {

class Queue : public QObject
{
    Q_OBJECT
public:
    static Queue* getInstance();
    Queue* clear();
    Queue* enqueue(std::shared_ptr<Executor> executor, bool needsPrevious = true);
    void start();
    bool wasSuccessful();

signals:
    void finished(bool success);
    void stepFinished(int stepIndex, bool success);

private:
    explicit Queue(QObject *parent = nullptr);
    void execNext();
    bool failed;
    int numberOfSteps;

    std::deque<std::pair<std::shared_ptr<Executor>, bool>> queue;
    std::shared_ptr<Executor> current;
};

}

#endif // QUEUE_H
