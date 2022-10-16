#include "filenamegenerator.h"
#include "../../model/project.h"
#include "../../command/fileobjectconsumer.h"
#include "configuration.h"
#include <QFileInfo>

namespace Pipeline { namespace Neutralise {

QString FileNameGenerator::getFileNameFor(Command::FileObject::Type type) const
{
  QString fileName;
  if (project)
  {
    using Type = Command::FileObject::Type;
    if (type == Type::GRO || type == Type::TOP)
    {
      QString inputFileName = fileObjectConsumer->getFileNameFor(type);

      QFileInfo fileInfo(inputFileName);
      fileName = fileInfo.absolutePath() + "/" +
        fileInfo.baseName() + "_neutralised." + fileInfo.suffix();
    }
  }

  return fileName;
}

} }
