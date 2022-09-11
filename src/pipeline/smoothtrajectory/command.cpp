#include "command.h"
#include "../../gromacsconfigfilegenerator.h"
#include "../../statusmessagesetter.h"
#include "../../model/project.h"
#include "../../command/fileobjectconsumer.h"
#include "../../appprovider.h"
#include "../../logforwarder.h"
#include "../../simulationstatuschecker.h"
#include "src/command/fileobject.h"
#include "ui/simulationstatus.h"

#include <QDebug>
#include <QDir>
#include <cmath>
#include <memory>

namespace Pipeline { namespace SmoothTrajectory {

Command::Command()
  : Executor()
{
}

void Command::doExecute()
{
  qDebug() << "Exec simulation";
  QString gmx = AppProvider::get("gmx");
  if (gmx.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  using Type = ::Command::FileObject::Type;
  QString inputStructure = fileObjectConsumer->getFileNameFor(Type::XTC);

  QString command = gmx + " filter";
  command += " -f " + inputStructure;
  command += " -s " + fileObjectConsumer->getFileNameFor(Type::GRO);
  command += " -ol " + fileNameGenerator->getFileNameFor(Type::XTC);
  command += " -nojump";
  command += " -all";
  command += " -nf 5";

  StatusMessageSetter::getInstance()->setMessage("Executing " + command);
  process.setWorkingDirectory(QFileInfo(inputStructure).absolutePath());
  process.start(command);
}

QString Command::getInputFileName() const
{
  using Type = ::Command::FileObject::Type;
  return fileObjectConsumer->getFileNameFor(Type::GRO);
}

bool Command::canExecute() const
{
  return QFile(getInputFileName()).exists();
}

} }
