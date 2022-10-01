#ifndef COMMAND_FILEOBJECTCONSUMER_H
#define COMMAND_FILEOBJECTCONSUMER_H

#include "fileobject.h"
#include <memory>
#include <QMap>
#include <QObject>

namespace Command {

class FileObjectConsumer : public QObject
{
  Q_OBJECT
public:
  FileObjectConsumer(const QMap<InputOutput::Category, QList<FileObject::Type>>& requiresMap);
  const QMap<InputOutput::Category, QList<FileObject::Type>>& requires() const;
  const QMap<InputOutput::Category, FileObject::Pointer>
  getConnectedTo() const;

  bool accepts(FileObject::Pointer fileObject);
  void connectTo(FileObject::Pointer fileObject);
  void disconnectFrom(FileObject::Pointer fileObject);
  QString getFileNameFor(FileObject::Type type) const;

signals:
  void connectedToChanged(
    FileObject::Pointer fileObject,
    InputOutput::Category connectedToCategory,
    FileObject::Pointer oldFileObject
    );

private:
  InputOutput::Category getCategoryFor(FileObject::Pointer fileObject);
  QMap<InputOutput::Category, FileObject::Pointer> connectedTo;

  QMap<InputOutput::Category, QList<FileObject::Type>> requiresMap;
};

}

#endif
