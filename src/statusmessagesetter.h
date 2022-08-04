#ifndef STATUSMESSAGESETTER_H
#define STATUSMESSAGESETTER_H

#include <QObject>

class StatusMessageSetter : public QObject
{
    Q_OBJECT
public:
    static StatusMessageSetter* getInstance();
    void setMessage(const QString& message);

signals:
    void messageChanged(const QString& message);

private:
    explicit StatusMessageSetter(QObject *parent = nullptr);

};

#endif // STATUSMESSAGESETTER_H
