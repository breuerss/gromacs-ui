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

  using Type = Command::FileObject::Type;
  const static QMap<Type, QString> suffixMap{
    { Type::LOG, ".log" },
    { Type::MDP, ".mdp" },
    { Type::GRO, ".gro" },
    { Type::XTC, ".xtc" },
    { Type::TRR, ".trr" },
    { Type::EDR, ".edr" },
    { Type::TPR, ".tpr" },
    { Type::TOP, ".top" },
  };

  if (suffixMap.contains(type))
  {
    const QString& projectPath = project->getProjectPath();

    auto conf = dynamic_cast<const Configuration*>(configuration);
    QString simulationType = conf->getTypeAsString();
    auto id = conf->property("id").toString();
    if (!id.isEmpty())
    {
      id = "-" + id;
    }
    fileName = projectPath + "/" + simulationType + id + "/" + simulationType;
    fileName += suffixMap[type];
  }

  return fileName;
}

} }
