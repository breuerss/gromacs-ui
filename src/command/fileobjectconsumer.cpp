#include "fileobjectconsumer.h"
#include "src/command/fileobject.h"
#include <memory>

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
  auto category = getCategoryFor(fileObject);
  if (category != FileObject::Category::Unknown)
  {
    connectedTo[category] = fileObject;
    emit connectedToChanged(fileObject);
  }
}

void FileObjectConsumer::disconnectFrom(std::shared_ptr<FileObject> fileObject)
{
  auto category = getCategoryFor(fileObject);
  if (connectedTo[category] == fileObject)
  {
    connectedTo.remove(category);
    emit connectedToChanged(nullptr);
  }
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
