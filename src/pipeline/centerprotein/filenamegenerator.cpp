#include "filenamegenerator.h"
#include "../../command/fileobjectconsumer.h"
#include <QFileInfo>

namespace Pipeline { namespace CenterProtein {

QString FileNameGenerator::getFileNameFor(Command::FileObject::Type type) const
{
  using FileObject = Command::FileObject;
  QString inputFileName = fileObjectConsumer->getFileNameFor(FileObject::Type::XTC);
  QFileInfo fileInfo(inputFileName);
  QString fileName = fileInfo.absolutePath() + "/" + fileInfo.baseName();
  switch (type)
  {
    case Command::FileObject::Type::GRO:
      fileName += "-center.gro";
      break;
    case Command::FileObject::Type::XTC:
      fileName += "-center.xtc";
      break;
    default:
      fileName = "";
  }

  return fileName;
}

} }
