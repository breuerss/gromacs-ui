#include "command.h"
#include "filenamegenerator.h"
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
  QString command = AppProvider::get("gmx");
  if (command.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  using Type = ::Command::FileObject::Type;
  QString inputStructure = fileObjectConsumer->getFileNameFor(Type::XTC);

  QStringList args("filter");
  args << "-f" << inputStructure;
  args << "-s" << fileObjectConsumer->getFileNameFor(Type::GRO);
  args << "-ol" << fileNameGenerator->getFileNameFor(Type::XTC);
  args << "-nojump";
  args << "-all";
  args << "-nf" << "5";

  StatusMessageSetter::getInstance()->setMessage("Executing " + command + " " + args.join(" "));
  process.setWorkingDirectory(QFileInfo(inputStructure).absolutePath());
  process.start(command, args);
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
