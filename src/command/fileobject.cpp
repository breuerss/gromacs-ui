#include "fileobject.h"

#include <QFile>

namespace Command {

FileObject::FileObject(const QString& newFileName, Type newType)
  : fileName(newFileName)
  , type(newType)
{
}

bool FileObject::exists() const
{
  return QFile(fileName).exists();
}

}


