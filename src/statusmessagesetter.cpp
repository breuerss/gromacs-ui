#include "statusmessagesetter.h"

#include <memory>
StatusMessageSetter* StatusMessageSetter::getInstance()
{
    static std::unique_ptr<StatusMessageSetter> instance;
    if (!instance)
    {
        instance.reset(new StatusMessageSetter);
    }
    return instance.get();
}

StatusMessageSetter::StatusMessageSetter(QObject *parent)
    : QObject{parent}
{

}

void StatusMessageSetter::setMessage(const QString& message)
{
    emit messageChanged(message);
}
