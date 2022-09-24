#include "fileobjectconsumer.h"
#include "src/command/fileobject.h"
#include "../undoredo/addconnectioncommand.h"
#include "../undoredo/removeconnectioncommand.h"
#include "../undoredo/stack.h"
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

void FileObjectConsumer::connectTo(std::shared_ptr<FileObject> fileObject, bool createUndoRedo)
{
  if (createUndoRedo)
  {
    UndoRedo::Stack::getInstance()
      ->push(new UndoRedo::AddConnectionCommand(fileObject, this));
  }
  else
  {
    auto category = getCategoryFor(fileObject);
    if (category != FileObject::Category::Unknown)
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
}

void FileObjectConsumer::disconnectFrom(std::shared_ptr<FileObject> fileObject, bool createUndoRedo)
{
  if (createUndoRedo)
  {
    UndoRedo::Stack::getInstance()
      ->push(new UndoRedo::RemoveConnectionCommand(fileObject, this));
  }
  else {
    auto category = getCategoryFor(fileObject);
    if (connectedTo.contains(category) && connectedTo[category] == fileObject)
    {
      connectedTo.remove(category);
      emit connectedToChanged(nullptr, category, fileObject);
    }
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

const QMap<FileObject::Category, std::shared_ptr<FileObject>>
FileObjectConsumer::getConnectedTo() const
{
  return connectedTo;
}

}
