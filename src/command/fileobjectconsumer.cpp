#include "fileobjectconsumer.h"
#include "../undoredo/addconnectioncommand.h"
#include "../undoredo/removeconnectioncommand.h"
#include "../undoredo/stack.h"
#include "inputoutput.h"
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

bool FileObjectConsumer::accepts(const Data& fileObject)
{
  return getCategoryFor(fileObject) != InputOutput::Category::Unknown;
}

void FileObjectConsumer::connectTo(const Data& data)
{
  auto category = getCategoryFor(data);
  if (category != InputOutput::Category::Unknown)
  {
    Data old;
    if (connectedTo.contains(category))
    {
      old = connectedTo[category];
    }
    connectedTo[category] = data;

    emit connectedToChanged(data, category, old);
  }
}

void FileObjectConsumer::disconnectFrom(const Data& data)
{
  if (!connectedTo.values().contains(data))
  {
    return;
  }

  auto category = getCategoryFor(data);
  if (connectedTo.contains(category) && connectedTo[category] == data)
  {
    connectedTo.remove(category);
    emit connectedToChanged(Data(), category, data);
  }
}

QString FileObjectConsumer::getFileNameFor(FileObject::Type type) const
{
  QString fileName;
  for (const auto& object : connectedTo.values())
  {
    if (Command::isSet<FileObject::Pointer>(object))
    {
      auto fileObject = std::get<FileObject::Pointer>(object);
      if (fileObject->type == type)
      {
        fileName = fileObject->getFileName();
        break;
      }
    }
  }

  return fileName;
}

InputOutput::Category FileObjectConsumer::getCategoryFor(const Data& data)
{
  InputOutput::Category category = InputOutput::Category::Unknown;


  if (Command::isSet<FileObject::Pointer>(data))
  {
    auto fileObject = std::get<FileObject::Pointer>(data);
    auto requireMap = requires();
    for (const auto& key: requireMap.keys())
    {
      if (requireMap[key].contains(fileObject->type))
      {
        category = key;
        break;
      }
    }
  }
  else if (std::holds_alternative<Config::Configuration::Pointer>(data))
  {
    category = InputOutput::Category::Configuration;
  }

  return category;
}

const QMap<InputOutput::Category, Data>
FileObjectConsumer::getConnectedTo() const
{
  return connectedTo;
}

}
