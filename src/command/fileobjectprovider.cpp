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

const QList<FileObject::Pointer>& FileObjectProvider::provides() const
{
  return providesList;
}

QList<FileObject::Pointer>& FileObjectProvider::provides()
{
  return providesList;
}

}
