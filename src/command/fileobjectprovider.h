#ifndef COMMAND_FILEOBJECTPROVIDER_H
#define COMMAND_FILEOBJECTPROVIDER_H

#include "fileobject.h"
#include <memory>

namespace Command {

class FileObjectProvider
{
public:
  virtual QList<std::shared_ptr<FileObject>> provides() const = 0;
};

}

#endif
