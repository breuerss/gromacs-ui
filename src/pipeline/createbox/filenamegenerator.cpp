#include "filenamegenerator.h"
#include "../../model/project.h"
#include "../../command/fileobjectconsumer.h"
#include "configuration.h"
#include <QFileInfo>

namespace Pipeline { namespace CreateBox {

using Type = ::Command::FileObject::Type;
QString FileNameGenerator::getFileNameFor(Type type) const
{
  QString fileName;
  if (type == Type::GRO && project)
  {
    QString inputFileName = fileObjectConsumer->getFileNameFor(type);
    QFileInfo fileInfo(inputFileName);

    fileName = fileInfo.absolutePath() + "/" +
      fileInfo.baseName() + "_boxed.gro";
  }

  return fileName;
}

} }
