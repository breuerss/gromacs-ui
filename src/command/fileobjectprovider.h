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
  const QList<std::shared_ptr<FileObject>>& provides() const;

private:
   QList<std::shared_ptr<FileObject>> providesList;
};

}

#endif
