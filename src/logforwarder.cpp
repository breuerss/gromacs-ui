#include "logforwarder.h"

#include <memory>

LogForwarder* LogForwarder::getInstance()
{
    static std::unique_ptr<LogForwarder> instance;
    if (!instance)
    {
        instance.reset(new LogForwarder);
    }

    return instance.get();
}

LogForwarder::LogForwarder(QObject *parent)
    : QObject{parent}
{

}

void LogForwarder::forward(const QString& message, QProcess::ProcessChannel channel)
{
    for (const QString& line : message.split("\n"))
    {
        emit addMessage(line, channel);
    }
}

void LogForwarder::listenTo(QProcess* process)
{
    connect(process, &QProcess::readyReadStandardError, [this, process] () {
        QString update = process->readAllStandardError();
        forward(update, QProcess::StandardError);
    });
    connect(process, &QProcess::readyReadStandardOutput, [this, process] () {
        QString update = process->readAllStandardOutput();
        forward(update, QProcess::StandardOutput);
    });
}

void LogForwarder::detach(QProcess* process)
{
    disconnect(process, &QProcess::readyReadStandardError, 0, 0);
    disconnect(process, &QProcess::readyReadStandardOutput, 0, 0);
}
