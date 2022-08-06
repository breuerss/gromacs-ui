#include "executor.h"
#include "../statusmessagesetter.h"

namespace Command {

Executor::Executor(QObject *parent)
    : QObject{parent}
    , mHasRun(false)
{
    connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this] (int, QProcess::ExitStatus) {
        mHasRun = true;
        QString command = process.program() + " " + process.arguments().join(" ");
        QString message("Error executing " + command);
        if (process.exitCode() == 0)
        {
            message = "Sucessfully executed " + command;
        }

        StatusMessageSetter::getInstance()->setMessage(message);
        emit finished();
    });
}

bool Executor::hasRun() const
{
    return mHasRun;
}

bool Executor::wasSuccessful() const
{
    return hasRun() &&
            process.exitStatus() == QProcess::NormalExit &&
            process.exitCode() == 0;
}

}
