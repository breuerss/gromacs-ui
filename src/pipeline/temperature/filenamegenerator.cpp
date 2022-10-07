#include "filenamegenerator.h"
#include "../../model/project.h"
#include "../../command/fileobjectconsumer.h"
#include <QFileInfo>

namespace Pipeline { namespace Temperature {

QString FileNameGenerator::getFileNameFor(Command::FileObject::Type type) const
{
  QString fileName;
  using Type = Command::FileObject::Type;
  if (type == Type::XVG && project)
  {
    QString inputFileName = fileObjectConsumer->getFileNameFor(Type::EDR);
    QFileInfo fileInfo(inputFileName);

    fileName = fileInfo.absolutePath() + "/temperature.xvg";
  }

  return fileName;
}

} }
