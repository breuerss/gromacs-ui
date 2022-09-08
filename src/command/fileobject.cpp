#include "fileobject.h"

#include <QFile>
#include <QMap>
#include <QJsonObject>

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
  const bool changed = fileName != newFileName;
  fileName = newFileName;

  if (changed)
  {
    fileNameChanged(fileName);
  }
}

const QString& FileObject::getFileName() const
{
  return fileName;
}

FileObject::Category FileObject::getCategoryFor(FileObject::Type type)
{
  static const QMap<Type, Category> categoryMap = {
    { Type::GRO, Category::Coordinates },
    { Type::PDB, Category::Coordinates },
    { Type::XTC, Category::Trajectory },
    { Type::TRR, Category::Trajectory },
  };

  return categoryMap[type];
}

QDataStream &operator<<(QDataStream &out, const FileObject::Pointer obj)
{
  out << obj->getFileName();

  return out;
}

QDataStream &operator>>(QDataStream &in, FileObject::Pointer obj)
{
  QString fileName;
  in >> fileName;

  obj->setFileName(fileName);

  return in;
}

QJsonObject &operator<<(QJsonObject &out, const FileObject::Pointer obj)
{
  out["fileName"] = obj->getFileName();

  return out;
}

QJsonObject &operator>>(QJsonObject &in, FileObject::Pointer obj)
{
  QString fileName;
  if (in.contains("fileName") && in["fileName"].isString())
  {
    fileName = in["fileName"].toString();
  }

  obj->setFileName(fileName);

  return in;
}

}


