#include "fileobject.h"

#include <QFile>
#include <QMap>

namespace Command {

FileObject::FileObject(Type newType)
  : type(newType)
{
}

bool FileObject::exists() const
{
  return QFile(fileName).exists();
}

void FileObject::setFileName(const QString& newFileName)
{
  fileName = newFileName;
}

FileObject::Category FileObject::getCategoryFor(FileObject::Type type)
{
  static const QMap<Type, Category> categoryMap = {
    { Type::GRO, Category::Coordinates },
    { Type::XTC, Category::Trajectory },
    { Type::TRR, Category::Trajectory },
  };

  return categoryMap[type];
}

}


