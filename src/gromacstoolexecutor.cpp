#include "gromacstoolexecutor.h"

#include "settings.h"
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include "statusmessagesetter.h"

void GromacsToolExecutor::execPdb2gmx(const std::shared_ptr<SystemSetup> systemSetup)
{
    Settings settings;
    QString command = settings.value(Settings::GMX_PATH).toString();
    command += " pdb2gmx";
    QProcess process;
    QString inputFile = systemSetup->getFilteredStructureFile();
    QFileInfo fileInfo(inputFile);

    QString outputFileName = fileInfo.baseName().replace("_filtered", "_processed") + ".gro";
    command += " -f " + inputFile;
    command += " -o " + outputFileName;
    command += " -water " + systemSetup->getWaterModel();
    command += " -ff " + systemSetup->getForceField();

    QString inputDirectory = fileInfo.absolutePath();
    QDir inputDir(inputDirectory);
    inputDir.cdUp();
    inputDir.mkdir("system");
    inputDir.cd("system");
    qDebug() << command;
    process.setWorkingDirectory(inputDir.absolutePath());
    process.start(command);
    process.waitForFinished();

    QString message("Error executing " + command);
    if (process.exitCode() == 0)
    {
        message = "Sucessfully executed " + command;
        systemSetup->setProcessedStructureFile(inputDir.absolutePath() + "/" + outputFileName);
    }

    StatusMessageSetter::getInstance()->setMessage(message);
}

void GromacsToolExecutor::execEditConf(const std::shared_ptr<SystemSetup> systemSetup)
{
    Settings settings;
    QString command = settings.value(Settings::GMX_PATH).toString();
    command += " editconf";
    QProcess process;
    QString inputFile = systemSetup->getProcessedStructureFile();
    QFileInfo fileInfo(inputFile);

    QString outputFile = fileInfo.baseName().replace("_processed", "_boxed") + ".gro";
    command += " -f " + inputFile;
    command += " -o " + outputFile;
    command += " -d " + QString::number(systemSetup->getDistance());
    command += " -bt " + systemSetup->getBoxType();

    QString inputDirectory = fileInfo.absolutePath();
    QDir inputDir(inputDirectory);
    qDebug() << command;
    process.setWorkingDirectory(inputDir.absolutePath());
    process.start(command);
    process.waitForFinished();

    QString message("Error executing " + command);
    if (process.exitCode() == 0)
    {
        message = "Sucessfully executed " + command;
        systemSetup->setBoxedStructureFile(inputDir.absolutePath() + "/" + outputFile);
    }

    StatusMessageSetter::getInstance()->setMessage(message);
}

void GromacsToolExecutor::execSolvate(const std::shared_ptr<SystemSetup> systemSetup)
{
    Settings settings;
    QString command = settings.value(Settings::GMX_PATH).toString();
    command += " solvate";
    QProcess process;
    QString inputFile = systemSetup->getBoxedStructureFile();
    QFileInfo fileInfo(inputFile);

    QString outputFile = fileInfo.baseName().replace("_boxed", "_solvated") + ".gro";
    command += " -cp " + inputFile;
    command += " -o " + outputFile;
    command += " -cs " + getWaterBoxFor(systemSetup->getWaterModel());
    command += " -p topol.top";

    QString inputDirectory = fileInfo.absolutePath();
    qDebug() << command;
    process.setWorkingDirectory(inputDirectory);
    process.start(command);
    process.waitForFinished();

    QString message("Error executing " + command);
    if (process.exitCode() == 0)
    {
        message = "Sucessfully executed " + command;
        systemSetup->setSolvatedStructureFile(inputDirectory + "/" + outputFile);
    }

    StatusMessageSetter::getInstance()->setMessage(message);
}

QString GromacsToolExecutor::getWaterBoxFor(const QString &solvent)
{
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
