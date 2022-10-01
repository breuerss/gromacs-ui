#include "fileobjectconsumer.h"
#include "../undoredo/addconnectioncommand.h"
#include "../undoredo/removeconnectioncommand.h"
#include "../undoredo/stack.h"
#include <memory>
#include <QDebug>

namespace Command {

FileObjectConsumer::FileObjectConsumer(
  const QMap<InputOutput::Category, QList<FileObject::Type>>& requiresMap
  )
  : requiresMap(requiresMap)
{
}

const QMap<InputOutput::Category, QList<FileObject::Type>>& FileObjectConsumer::requires() const 
{
  return requiresMap;
}

bool FileObjectConsumer::accepts(std::shared_ptr<FileObject> fileObject)
{
  return getCategoryFor(fileObject) != InputOutput::Category::Unknown;
}

void FileObjectConsumer::connectTo(std::shared_ptr<FileObject> fileObject)
{
  auto category = getCategoryFor(fileObject);
  if (category != InputOutput::Category::Unknown)
  {
    std::shared_ptr<FileObject> old;
    if (connectedTo.contains(category))
    {
      old = connectedTo[category];
    }
    connectedTo[category] = fileObject;

    emit connectedToChanged(fileObject, category, old);
  }
}

void FileObjectConsumer::disconnectFrom(FileObject::Pointer fileObject)
{
  if (!connectedTo.values().contains(fileObject))
  {
    return;
  }

  auto category = getCategoryFor(fileObject);
  if (connectedTo.contains(category) && connectedTo[category] == fileObject)
  {
    connectedTo.remove(category);
    emit connectedToChanged(nullptr, category, fileObject);
  }
}

QString FileObjectConsumer::getFileNameFor(FileObject::Type type) const
{
  QString fileName;
  for (auto object : connectedTo.values())
  {
    if (object->type == type)
    {
      fileName = object->getFileName();
      break;
    }
  }

  return fileName;
}

InputOutput::Category FileObjectConsumer::getCategoryFor(std::shared_ptr<FileObject> fileObject)
{
  InputOutput::Category category = InputOutput::Category::Unknown;

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

const QMap<InputOutput::Category, std::shared_ptr<FileObject>>
FileObjectConsumer::getConnectedTo() const
{
  return connectedTo;
}

}
