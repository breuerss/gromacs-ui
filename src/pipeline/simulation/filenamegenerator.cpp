#include "filenamegenerator.h"
#include "../../model/project.h"
#include "../../command/fileobjectconsumer.h"
#include "configuration.h"
#include <QFileInfo>

namespace Pipeline { namespace Simulation {

QString FileNameGenerator::getFileNameFor(Command::FileObject::Type type) const
{
  QString fileName;

  if (!project)
  {
    return fileName;
  }

  QString projectPath = project->getProjectPath();
  QString simulationType = std::dynamic_pointer_cast<Configuration>(configuration)
    ->getTypeAsString();
  fileName = projectPath + "/" + simulationType + "/" + simulationType;
  switch (type)
  {
    case Command::FileObject::Type::LOG:
      fileName += ".log";
      break;
    case Command::FileObject::Type::MDP:
      fileName += ".mdp";
      break;
    case Command::FileObject::Type::GRO:
      fileName += ".gro";
      break;
    case Command::FileObject::Type::XTC:
      fileName += ".xtc";
      break;
    case Command::FileObject::Type::TRR:
      fileName += ".trr";
      break;
    case Command::FileObject::Type::EDR:
      fileName += ".edr";
      break;
    case Command::FileObject::Type::TPR:
      fileName += ".tpr";
      break;
    case Command::FileObject::Type::TOP:
      fileName += ".top";
      break;
    default:
      fileName = "";
  }

  return fileName;
}

} }
