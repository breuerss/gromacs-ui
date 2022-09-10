#include "filenamegenerator.h"
#include "../../model/project.h"
#include "../../command/fileobjectconsumer.h"
#include "configuration.h"
#include <QFileInfo>

namespace Pipeline { namespace CreateGromacsModel {

QString FileNameGenerator::getFileNameFor(Command::FileObject::Type type) const
{
  QString fileName;
  using Type = Command::FileObject::Type;
  if (type == Type::GRO && project)
  {
    QString inputFileName = fileObjectConsumer
      ->getFileNameFor(Type::PDB);

    QFileInfo fileInfo(inputFileName);
    fileName = fileInfo.absolutePath() + "/" +
      fileInfo.baseName() + "_model.gro";
  }

  return fileName;
}

} }
