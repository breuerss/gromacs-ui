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
    virtual void exec() = 0;

    bool hasRun() const;
    bool wasSuccessful() const;

signals:
    void finished();

protected:
    QProcess process;

private:
    bool mHasRun;

};

}

#endif // EXECUTOR_H
