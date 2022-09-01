#include "inputfilenamegenerator.h"
#include "../model/project.h"
#include "../config/pdb.h"

namespace Command {

QString InputFileNameGenerator::getFileNameFor(FileObject::Type type) const
{
  QString fileName;
  if (type == FileObject::Type::PDB)
  {
    auto pdbCode = std::dynamic_pointer_cast<Config::Pdb>(configuration)
      ->property("pdbCode").toString();
    fileName = project->getProjectPath();
    fileName += QString("input/%1.pdb").arg(pdbCode);
  }

  return fileName;
}

}

