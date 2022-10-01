#include "command.h"
#include "../../statusmessagesetter.h"
#include "../../command/fileobjectconsumer.h"
#include "../timestepcontrol/configuration.h"
#include "../../appprovider.h"
#include "../../logforwarder.h"
#include "../../command/fileobject.h"
#include "filenamegenerator.h"

#include <QDebug>
#include <QDir>
#include <cmath>
#include <memory>

namespace Pipeline { namespace CenterProtein {

Command::Command()
  : Executor()
{
}

void Command::doExecute()
{
  QString gmx = AppProvider::get("gmx");
  if (gmx.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  using Type = ::Command::FileObject::Type;
  QString inputStructure = fileObjectConsumer->getFileNameFor(Type::XTC);

  QString command = gmx + " trjconv";
  command += " -f " + inputStructure;
  command += " -s " + fileObjectConsumer->getFileNameFor(Type::GRO);
  command += " -o " + fileNameGenerator->getFileNameFor(Type::XTC);
  command += " -center";
  command += " -pbc nojump";

  const auto& connectedTo = fileObjectConsumer->getConnectedTo();
  const auto& timeStepControlPort = connectedTo[::Command::InputOutput::Category::Configuration];
  if (std::holds_alternative<Config::Configuration::Pointer>(timeStepControlPort))
  {
    auto timeStepControlPointer = std::get<Config::Configuration::Pointer>(timeStepControlPort);

    if (timeStepControlPointer)
    {
      auto timeStepControl = std::dynamic_pointer_cast<TimeStepControl::Configuration>(timeStepControlPointer);

      command += " " + timeStepControl->getForCommand();
    }
  }

  StatusMessageSetter::getInstance()->setMessage("Executing " + command);
  process.setWorkingDirectory(QFileInfo(inputStructure).absolutePath());
  process.start(command);
  process.write("1 0");
  process.closeWriteChannel();
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
