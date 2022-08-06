#include "createbox.h"

#include "../settings.h"
#include "../statusmessagesetter.h"
#include <QFileInfo>
#include <QDebug>

namespace Command {

CreateBox::CreateBox(std::shared_ptr<SystemSetup> newSystemSetup, QObject *parent)
    : Executor{parent}
    , systemSetup(newSystemSetup)
{

}

void CreateBox::exec()
{
    qDebug() << "Create Box";
    Settings settings;
    QString command = settings.value(Settings::GMX_PATH).toString();
    if (command.isEmpty())
    {
        QString message("Path to 'gmx' command is not set.");
        StatusMessageSetter::getInstance()->setMessage(message);
        return;
    }

    command += " editconf";
    QString inputFile = systemSetup->getProcessedStructureFile();
    QFileInfo fileInfo(inputFile);

    QString outputFile = fileInfo.baseName().replace("_processed", "_boxed") + ".gro";
    command += " -f " + inputFile;
    command += " -o " + outputFile;
    command += " -d " + QString::number(systemSetup->getDistance());
    command += " -bt " + systemSetup->getBoxType();

    QString inputDirectory = fileInfo.absolutePath();
    StatusMessageSetter::getInstance()->setMessage("Executing " + command);
    process.setWorkingDirectory(inputDirectory);
    process.start(command);
    systemSetup->setBoxedStructureFile(inputDirectory + "/" + outputFile);
}

}
