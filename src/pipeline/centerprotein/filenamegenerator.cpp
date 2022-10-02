#include "filenamegenerator.h"
#include "../../command/fileobjectconsumer.h"
#include <QFileInfo>

namespace Pipeline { namespace CenterProtein {

using Type = ::Command::FileObject::Type;
QString FileNameGenerator::getFileNameFor(Type type) const
{
  QString inputFileName = fileObjectConsumer->getFileNameFor(Type::XTC);
  QFileInfo fileInfo(inputFileName);
  QString fileName = fileInfo.absolutePath() + "/" + fileInfo.baseName();
  switch (type)
  {
    case Type::GRO:
      fileName += "-center.gro";
      break;
    case Type::XTC:
      fileName += "-center.xtc";
      break;
    default:
      fileName = "";
  }

  return fileName;
}

} }
