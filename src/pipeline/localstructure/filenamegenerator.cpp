#include "filenamegenerator.h"
#include "../../model/project.h"
#include "configuration.h"
#include <QFileInfo>

namespace Pipeline { namespace LocalStructure {

QString FileNameGenerator::getFileNameFor(Command::FileObject::Type type) const
{
  QString fileName;
  if (type == Command::FileObject::Type::PDB && project)
  {
    auto structureFileName = dynamic_cast<const Configuration*>(configuration)
      ->property("fileName").toString();
    fileName = project->getProjectPath() + "/";
    fileName += QString("input/%1.pdb").arg(QFileInfo(structureFileName).baseName());
  }

  return fileName;
}

} }
