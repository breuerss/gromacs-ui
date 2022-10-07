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
  QString command = AppProvider::get("gmx");
  if (command.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  using Type = ::Command::FileObject::Type;
  QString inputStructure = fileObjectConsumer->getFileNameFor(Type::XTC);

  QStringList args("trjconv");
  args << "-f" << inputStructure;
  args << "-s" << fileObjectConsumer->getFileNameFor(Type::GRO);
  args << "-o" << fileNameGenerator->getFileNameFor(Type::XTC);
  args << "-center";
  args << "-pbc" << "nojump";

  const auto& connectedTo = fileObjectConsumer->getConnectedTo();
  const auto& timeStepControlPort = connectedTo[::Command::InputOutput::Category::Configuration];
  if (::Command::isSet<Config::Configuration::Pointer>(timeStepControlPort))
  {
    auto timeStepControlPointer = std::get<Config::Configuration::Pointer>(timeStepControlPort);

    if (timeStepControlPointer)
    {
      auto timeStepControl = std::dynamic_pointer_cast<TimeStepControl::Configuration>(timeStepControlPointer);

      args << timeStepControl->getArgsForCommand();
    }
  }

  StatusMessageSetter::getInstance()->setMessage("Executing " + command);
  process.setWorkingDirectory(QFileInfo(inputStructure).absolutePath());
  process.start(command, args);
  process.write("Protein System");
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
