#ifndef COMMAND_FILEOBJECTPROVIDER_H
#define COMMAND_FILEOBJECTPROVIDER_H

#include "fileobject.h"
#include "types.h"
#include <memory>
#include <QList>

namespace Command {

class FileObjectProvider
{
public:
  FileObjectProvider(const QList<FileObject::Type>& fileTypes);
  explicit FileObjectProvider(const QList<Data>& newProvidesList);
  const QList<Data>& provides() const;
  QList<Data>& provides();

private:
   QList<Data> providesList;
};

}

#endif
