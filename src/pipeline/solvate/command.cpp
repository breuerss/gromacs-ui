#include "command.h"
#include "configuration.h"
#include "filenamegenerator.h"

#include "../../appprovider.h"
#include "../../statusmessagesetter.h"
#include "../../command/fileobjectconsumer.h"

#include <QFileInfo>
#include <QDebug>

namespace Pipeline { namespace Solvate {

void Command::doExecute()
{
  QString command = AppProvider::get("gmx");
  if (command.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  QStringList args("solvate");
  QString inputFile = getInputFilename();

  QString outputFile = fileNameGenerator->getFileNameFor(Type::GRO);
  args << "-cp" << inputFile;
  args << "-o" << outputFile;
  args << "-cs" << getWaterBoxFor(getWaterSolventFromTopology());

  QString outputTopology = fileNameGenerator->getFileNameFor(Type::TOP);
  QFile(fileObjectConsumer->getFileNameFor(Type::TOP)).copy(outputTopology);
  args << "-p" << outputTopology;

  QFileInfo fileInfo(inputFile);
  QString inputDirectory = fileInfo.absolutePath();
  process.setWorkingDirectory(inputDirectory);
  StatusMessageSetter::getInstance()->setMessage("Executing command " + command + " " + args.join(" "));
  process.start(command, args);
}

bool Command::canExecute() const
{
  return QFile(getInputFilename()).exists() &&
    QFile(fileObjectConsumer->getFileNameFor(Type::TOP)).exists();
}

QString Command::getInputFilename() const
{
  return fileObjectConsumer->getFileNameFor(Type::GRO);
}

QString Command::getWaterSolventFromTopology()
{
  QFile topol(fileObjectConsumer->getFileNameFor(Type::TOP));
  if (topol.open(QIODevice::ReadOnly))
  {

    QTextStream in(&topol);
    while (!in.atEnd())
    {
      QString line = in.readLine();
      if (line.startsWith("#include")) {
        if (line.contains("tip4p")) {
          return "tip4p";
        } else if (line.contains("tip5p")) {
          return "tip5p";
        }
      }
    }
    topol.close();
  }

  // for SPC or TIP3P
  return "spc";
}

QString Command::getWaterBoxFor(const QString& solvent)
{
  // for SPC or TIP3P
  QString waterBox = "spc216.gro";
  if (solvent == "tip4p")
  {
    waterBox = "tip4p.gro";
  }
  else if (solvent == "tip5p")
  {
    waterBox = "tip5p.gro";
  }

  return waterBox;
}

} }
