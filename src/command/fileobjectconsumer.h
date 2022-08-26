#ifndef COMMAND_FILEOBJECTCONSUMER_H
#define COMMAND_FILEOBJECTCONSUMER_H

#include "fileobject.h"
#include <memory>
#include <QMap>
#include <QObject>

namespace Command {

class FileObjectConsumer : private QObject
{
  Q_OBJECT
public:
  FileObjectConsumer(const QMap<FileObject::Category, QList<FileObject::Type>>& requiresMap);
  const QMap<FileObject::Category, QList<FileObject::Type>>& requires() const;

  bool accepts(std::shared_ptr<FileObject> fileObject);
  void connectTo(std::shared_ptr<FileObject> fileObject);
  void disconnectFrom(std::shared_ptr<FileObject> fileObject);

signals:
  void connectedToChanged(std::shared_ptr<FileObject> fileObject);

private:
  FileObject::Category getCategoryFor(std::shared_ptr<FileObject> fileObject);
  QMap<FileObject::Category, std::shared_ptr<FileObject>> connectedTo;

  QMap<FileObject::Category, QList<FileObject::Type>> requiresMap;
};

}

#endif
