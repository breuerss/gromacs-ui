#ifndef COMMAND_FILEOBJECTPROVIDER_H
#define COMMAND_FILEOBJECTPROVIDER_H

#include "fileobject.h"
#include <memory>
#include <QList>

namespace Command {

class FileObjectProvider
{
public:
  FileObjectProvider(const QList<FileObject::Type>& fileTypes);
  const QList<FileObject::Pointer>& provides() const;
  QList<FileObject::Pointer>& provides();

private:
   QList<FileObject::Pointer> providesList;
};

}

#endif
