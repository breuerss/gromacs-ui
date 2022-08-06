#include "gromacstoolexecutor.h"

#include "settings.h"
#include "statusmessagesetter.h"
#include "gromacsconfigfilegenerator.h"
#include "model/project.h"

#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

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
    process.setWorkingDirectory(inputDirectory);
    process.start(command);
    process.waitForFinished();

    QString message("Error executing " + command);
    if (process.exitCode() == 0)
    {
        message = "Sucessfully executed " + command;
        systemSetup->setProcessedStructureFile(inputDirectory + "/" + outputFileName);
    }

    qDebug() << message;
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
    qDebug() << command;
    process.setWorkingDirectory(inputDirectory);
    process.start(command);
    process.waitForFinished();

    QString message("Error executing " + command);
    if (process.exitCode() == 0)
    {
        message = "Sucessfully executed " + command;
        systemSetup->setBoxedStructureFile(inputDirectory + "/" + outputFile);
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
    StatusMessageSetter::getInstance()->setMessage("Running command " + command);
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

void GromacsToolExecutor::execMdrun(const std::shared_ptr<Project> project, int stepIndex)
{
    QDir dir(project->getProjectPath());
    const auto& steps = project->getSteps();
    std::shared_ptr<Step> step = steps[stepIndex];

    QString stepType = step->getDirectory();
    dir.mkdir(stepType);
    dir.cd(stepType);
    QString mdpFile = dir.absolutePath() + "/" + stepType + ".mdp";
    GromacsConfigFileGenerator::generate(step, mdpFile);
    qDebug() << mdpFile;

    QProcess process;
    process.setWorkingDirectory(dir.absolutePath());


    QString inputStructure = project->getSystemSetup()->getSolvatedStructureFile();
    QFileInfo systemPath(inputStructure);
    if (stepIndex > 0)
    {
        QString prevStepType = steps[stepIndex - 1]->getDirectory();
        inputStructure = dir.absolutePath() + "/../" + prevStepType + "/" + prevStepType + ".gro";
    }
    QString command = "gmx grompp";
    command += " -f " + mdpFile;
    command += " -c " + inputStructure;
    command += " -p " + systemPath.absolutePath() + "/topol.top";
    command += " -maxwarn 2 ";
    command += " -o " + stepType + ".tpr";

    qDebug() << "executing" << command;
    process.start(command);
    process.waitForFinished(900000);

    if (process.exitCode() != 0)
    {
        StatusMessageSetter::getInstance()->setMessage("Could not execute " + command);
        return;
    }

    command = "gmx mdrun -v -deffnm " + stepType;
    qDebug() << "executing" << command;
    QObject::connect(&process, &QProcess::readyReadStandardOutput, [&process] () {
            qDebug() << "readRead";
        QString message = process.readLine();
        while (!message.isEmpty())
        {
            qDebug() << message;
            StatusMessageSetter::getInstance()->setMessage(message);
            message = process.readLine();
        }
    });
    process.start(command);
    process.waitForFinished(900000);
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
