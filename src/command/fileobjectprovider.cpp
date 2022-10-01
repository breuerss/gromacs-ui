#include "fileobjectprovider.h"
#include <memory>

namespace Command {

FileObjectProvider::FileObjectProvider(const QList<FileObject::Type>& types)
{
  for (const auto& type: types)
  {
    providesList << std::make_shared<FileObject>(type);
  }
}

FileObjectProvider::FileObjectProvider(const QList<Data>& newProvidesList)
  : providesList(newProvidesList)
{
}

const QList<Data>& FileObjectProvider::provides() const
{
  return providesList;
}

QList<Data>& FileObjectProvider::provides()
{
  return providesList;
}

}
