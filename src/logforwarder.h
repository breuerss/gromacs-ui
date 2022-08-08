#ifndef LOGFORWARDER_H
#define LOGFORWARDER_H

#include <QObject>
#include <QProcess>

class LogForwarder : public QObject
{
  Q_OBJECT
public:
  static LogForwarder* getInstance();
  void forward(const QString& message, QProcess::ProcessChannel channel);
  void listenTo(QProcess* process);
  void detach(QProcess* process);

signals:
  void addMessage(const QString& message, QProcess::ProcessChannel channel);

private:
  explicit LogForwarder(QObject *parent = nullptr);
};

#endif // LOGFORWARDER_H
