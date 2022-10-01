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
  const QMap<InputOutput::Category, std::shared_ptr<FileObject>>
  getConnectedTo() const;

  bool accepts(std::shared_ptr<FileObject> fileObject);
  void connectTo(std::shared_ptr<FileObject> fileObject, bool createUndoRedo = true);
  void disconnectFrom(std::shared_ptr<FileObject> fileObject, bool createUndoRedo = true);
  QString getFileNameFor(FileObject::Type type) const;

signals:
  void connectedToChanged(
    std::shared_ptr<FileObject> fileObject,
    InputOutput::Category connectedToCategory,
    std::shared_ptr<FileObject> oldFileObject
    );

private:
  InputOutput::Category getCategoryFor(std::shared_ptr<FileObject> fileObject);
  QMap<InputOutput::Category, std::shared_ptr<FileObject>> connectedTo;

  QMap<InputOutput::Category, QList<FileObject::Type>> requiresMap;
};

}

#endif
