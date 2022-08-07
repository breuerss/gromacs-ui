#include "neutralise.h"

#include "../settings.h"
#include "../statusmessagesetter.h"
#include "../logforwarder.h"

#include <QDebug>
#include <QFileInfo>

namespace Command {

Neutralise::Neutralise(std::shared_ptr<SystemSetup> newSystemSetup, QObject *parent)
    : Executor{parent}
    , systemSetup(newSystemSetup)
{

}

void Neutralise::exec()
{
    qDebug() << "Neutralise";
    Settings settings;
    QString command = settings.value(Settings::GMX_PATH).toString();
    if (command.isEmpty())
    {
        QString message("Path to 'gmx' command is not set.");
        StatusMessageSetter::getInstance()->setMessage(message);
        return;
    }

    QString solvatedStructure = systemSetup->getSolvatedStructureFile();
    QFileInfo fileInfo(solvatedStructure);

    QString ionsBasePath = fileInfo.absolutePath() + "/ions";
    QString ionsMdpPath = ionsBasePath + ".mdp";
    QFile ionsMdp(ionsMdpPath);
    ionsMdp.open(QFile::WriteOnly);
    ionsMdp.close();

    QString inputDirectory = fileInfo.absolutePath();

    QProcess prepCommand;
    prepCommand.setWorkingDirectory(inputDirectory);
    LogForwarder::getInstance()->listenTo(&prepCommand);
    QString ionsTprPath = ionsBasePath + ".tpr";
    QString createIonsTprs = command + " grompp";
    createIonsTprs += " -f " + ionsMdpPath;
    createIonsTprs += " -c " + solvatedStructure;
    createIonsTprs += " -p topol.top";
    createIonsTprs += " -o " + ionsTprPath;

    StatusMessageSetter::getInstance()->setMessage("Executing command " + createIonsTprs);

    prepCommand.start(createIonsTprs);
    prepCommand.waitForFinished();
    if (prepCommand.exitCode() != 0)
    {
        qDebug() << prepCommand.exitCode();
        StatusMessageSetter::getInstance()->setMessage("Failed executing " + createIonsTprs);
    }
    LogForwarder::getInstance()->detach(&prepCommand);

    command += " genion";

    QString outputFile = fileInfo.baseName().replace("_solvated", "_neutralised") + ".gro";
    command += " -s " + ionsTprPath;
    command += " -o " + outputFile;
    command += " -conc " + QString::number(systemSetup->getIonContration());
    command += " -p topol.top";

    const QString& positiveIon = systemSetup->getPositiveIon();
    command += " -pname " + positiveIon;
    static const QMap<QString, int> positiveIonCharge = {
        { "MG", 2 },
        { "CA", 2 },
        { "LI", 1 },
        { "NA", 1 },
        { "K ", 1 },
        { "RB", 1 },
        { "CS", 1 },
    };
    command += " -pq " + QString::number(positiveIonCharge[positiveIon]);
    command += " -nname " + systemSetup->getNegativeIon();
    command += " -nq -1";
    command += " -neutral";

    process.setWorkingDirectory(inputDirectory);
    StatusMessageSetter::getInstance()->setMessage("Executing command " + command);
    process.start("bash", QStringList() << "-c" << "echo SOL|" + command);
    systemSetup->setNeutralisedStructureFile(inputDirectory + "/" + outputFile);
}


}
