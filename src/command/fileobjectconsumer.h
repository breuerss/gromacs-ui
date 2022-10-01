#ifndef COMMAND_FILEOBJECTCONSUMER_H
#define COMMAND_FILEOBJECTCONSUMER_H

#include "fileobject.h"
#include "types.h"
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
  const QMap<InputOutput::Category, Data>
  getConnectedTo() const;

  bool accepts(const Data& fileObject);
  void connectTo(const Data& fileObject);
  void disconnectFrom(const Data& fileObject);
  QString getFileNameFor(FileObject::Type type) const;

signals:
  void connectedToChanged(
    const Data& fileObject,
    InputOutput::Category connectedToCategory,
    const Data& oldFileObject
    );

private:
  InputOutput::Category getCategoryFor(const Data& fileObject);
  QMap<InputOutput::Category, Data> connectedTo;

  QMap<InputOutput::Category, QList<FileObject::Type>> requiresMap;
};

}

#endif
