#include "filenamegenerator.h"
#include "../../model/project.h"
#include "../../command/fileobjectconsumer.h"
#include "configuration.h"
#include <QFileInfo>

namespace Pipeline { namespace CreateGromacsModel {

QString FileNameGenerator::getFileNameFor(Command::FileObject::Type type) const
{
  QString fileName;
  if (!project)
  {
    return fileName;
  }

  using Type = Command::FileObject::Type;
  QString inputFileName = fileObjectConsumer
    ->getFileNameFor(Type::PDB);
  QFileInfo fileInfo(inputFileName);
  switch(type)
  {
    case Type::GRO:
      fileName = fileInfo.absolutePath() + "/" +
        fileInfo.baseName() + "_model.gro";
      break;
    case Type::TOP:
      fileName = fileInfo.absolutePath() + "/topol.top";
      break;
    default:
      fileName = "";
  }

  return fileName;
}

} }
