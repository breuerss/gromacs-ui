#ifndef FILESERVER_H
#define FILESERVER_H

#include <QObject>
#include <QProcess>
#include <QFileSystemWatcher>
#include "settings.h"

class FileServer : public QObject
{
  Q_OBJECT
public:
  static FileServer* getInstance();
  QString getServerUrl();
  QString getUrlForFile(const QString& absFilePath);

  ~FileServer();

  private slots:
  void start();

private:
  FileServer();

  QProcess servingProcess;
  QFileSystemWatcher watcher;
  Settings settings;
  int port;

};

#endif // FILESERVER_H
