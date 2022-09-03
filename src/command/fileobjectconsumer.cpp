#include "fileobjectconsumer.h"
#include "src/command/fileobject.h"
#include <memory>
#include <QDebug>

namespace Command {

FileObjectConsumer::FileObjectConsumer(
  const QMap<FileObject::Category, QList<FileObject::Type>>& requiresMap
  )
  : requiresMap(requiresMap)
{
}

const QMap<FileObject::Category, QList<FileObject::Type>>& FileObjectConsumer::requires() const 
{
  return requiresMap;
}

bool FileObjectConsumer::accepts(std::shared_ptr<FileObject> fileObject)
{
  return getCategoryFor(fileObject) != FileObject::Category::Unknown;
}

void FileObjectConsumer::connectTo(std::shared_ptr<FileObject> fileObject)
{
  qDebug() << "connectto" << __PRETTY_FUNCTION__;
  auto category = getCategoryFor(fileObject);
  if (category != FileObject::Category::Unknown)
  {
    auto old = connectedTo[category];
    connectedTo[category] = fileObject;
    emit connectedToChanged(fileObject, old);
  }
}

void FileObjectConsumer::disconnectFrom(std::shared_ptr<FileObject> fileObject)
{
  auto category = getCategoryFor(fileObject);
  if (connectedTo[category] == fileObject)
  {
    connectedTo.remove(category);
    emit connectedToChanged(nullptr, fileObject);
  }
}

QString FileObjectConsumer::getFileNameFor(FileObject::Type type) const
{
  qDebug() << __PRETTY_FUNCTION__;
  QString fileName;
  for (auto object : connectedTo.values())
  {
    qDebug() << "loop" << object.get();
    if (object->type == type)
    {
      fileName = object->getFileName();
      break;
    }
  }

  return fileName;
}

FileObject::Category FileObjectConsumer::getCategoryFor(std::shared_ptr<FileObject> fileObject)
{
  FileObject::Category category = FileObject::Category::Unknown;

  auto requireMap = requires();
  for (const auto& key: requireMap.keys())
  {
    if (requireMap[key].contains(fileObject->type))
    {
      category = key;
      break;
    }
  }

  return category;
}

}
