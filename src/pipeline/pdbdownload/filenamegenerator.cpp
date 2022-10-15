#include "filenamegenerator.h"
#include "../../model/project.h"
#include "configuration.h"

namespace Pipeline { namespace PdbDownload {

QString FileNameGenerator::getFileNameFor(Command::FileObject::Type type) const
{
  QString fileName;
  if (type == Command::FileObject::Type::PDB && project)
  {
    auto pdbCode = dynamic_cast<const Configuration*>(configuration)
      ->property("pdbCode").toString();
    fileName = project->getProjectPath() + "/";
    fileName += QString("input/%1.pdb").arg(pdbCode);
  }

  return fileName;
}

} }
