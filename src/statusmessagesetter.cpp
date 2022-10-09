#include "statusmessagesetter.h"

#include <memory>
#include <QDebug>

StatusMessageSetter* StatusMessageSetter::getInstance()
{
  static StatusMessageSetter instance;
  return &instance;
}

StatusMessageSetter::StatusMessageSetter(QObject *parent)
  : QObject{parent}
{

}

void StatusMessageSetter::setMessage(const QString& message)
{
  qDebug() << message;
  emit messageChanged(message);
}
