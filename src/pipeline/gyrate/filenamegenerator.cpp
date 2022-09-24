#include "filenamegenerator.h"
#include "../../model/project.h"
#include "../../command/fileobjectconsumer.h"
#include <QFileInfo>

namespace Pipeline { namespace Gyrate {

QString FileNameGenerator::getFileNameFor(Command::FileObject::Type type) const
{
  QString fileName;
  using Type = Command::FileObject::Type;
  if (type == Type::XVG && project)
  {
    QString inputFileName = fileObjectConsumer->getFileNameFor(Type::XTC);
    QFileInfo fileInfo(inputFileName);

    fileName = fileInfo.absolutePath() + "/gyrate.xvg";
  }

  return fileName;
}

} }
