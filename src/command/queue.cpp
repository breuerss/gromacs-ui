#include "queue.h"

#include <memory>
#include <QDebug>

namespace Command {

Queue::Queue(QObject *parent)
    : QObject{parent}
    , failed(false)
{
}

Queue* Queue::clear()
{
    queue.clear();
    if (current)
    {
        disconnect(current.get(), 0, 0, 0);
        current.reset();
    }
    return this;
}

Queue* Queue::enqueue(std::shared_ptr<Executor> executor, bool needsPrevious)
{
    queue.push_back({executor, needsPrevious});

    return this;
}

void Queue::start()
{
    qDebug() << __PRETTY_FUNCTION__;
    numberOfSteps = queue.size();
    execNext();
}

bool Queue::wasSuccessful()
{
    return !failed;
}

void Queue::execNext()
{
    qDebug() << __PRETTY_FUNCTION__;
    if (queue.size() > 0)
    {
        qDebug() << "queue has elements" << queue.size();
        auto nextCommand = queue.front();
        bool needsPrevious = nextCommand.second;

        if (
                !needsPrevious ||
                (needsPrevious && (!current || (current && current->wasSuccessful()))))
        {
            qDebug() << "can run";
            current = nextCommand.first;
            connect(current.get(), &Executor::finished, [this] () {
                disconnect(current.get(), 0, 0, 0);
                emit stepFinished(numberOfSteps - queue.size() - 1, current->wasSuccessful());
                execNext();
            });
            queue.pop_front();
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
        emit finished(!failed);
    }
}

}
