#include "filenamegenerator.h"
#include "../../model/project.h"
#include "../../command/fileobjectconsumer.h"
#include "configuration.h"
#include <QFileInfo>

namespace Pipeline { namespace Neutralise {

QString FileNameGenerator::getFileNameFor(Command::FileObject::Type type) const
{
  QString fileName;
  if (type == Command::FileObject::Type::GRO && project)
  {
    QString inputFileName = fileObjectConsumer->getFileNameFor(type);

    QFileInfo fileInfo(inputFileName);
    fileName = fileInfo.absolutePath() + "/" +
      fileInfo.baseName() + "_neutralised.gro";
  }

  return fileName;
}

} }
