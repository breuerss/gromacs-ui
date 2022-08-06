#include "runsimulation.h"
#include "../gromacsconfigfilegenerator.h"
#include "../statusmessagesetter.h"
#include "../model/systemsetup.h"
#include "../settings.h"

#include <QDebug>
#include <QDir>

namespace Command {

RunSimulation::RunSimulation(std::shared_ptr<Project> project, int stepIndex, QObject *parent)
    : Executor(parent)
    , project(project)
    , stepIndex(stepIndex)
{

}

void RunSimulation::exec()
{
    qDebug() << "Create model";
    QString gmx = getGmx();
    if (gmx.isEmpty())
    {
        QString message("Path to 'gmx' command is not set.");
        StatusMessageSetter::getInstance()->setMessage(message);
        return;
    }

    QDir dir(project->getProjectPath());
    const auto& steps = project->getSteps();
    std::shared_ptr<Step> step = steps[stepIndex];

    QString stepType = step->getDirectory();
    dir.mkdir(stepType);
    dir.cd(stepType);
    QString mdpFile = dir.absolutePath() + "/" + stepType + ".mdp";
    GromacsConfigFileGenerator::generate(step, mdpFile);
    qDebug() << mdpFile;

    QString inputStructure = project->getSystemSetup()->getNeutralisedStructureFile();
    QFileInfo systemPath(inputStructure);
    if (stepIndex > 0)
    {
        QString prevStepType = steps[stepIndex - 1]->getDirectory();
        inputStructure = dir.absolutePath() + "/../" + prevStepType + "/" + prevStepType + ".gro";
    }

    if (!execGrompp(
                mdpFile,
                inputStructure,
                systemPath.absolutePath() + "/topol.top",
                stepType + ".tpr",
                dir.absolutePath()
                ))
    {
        return;
    }

    const QString command = gmx + " mdrun -v -deffnm " + stepType;
    qDebug() << "executing" << command;
    StatusMessageSetter::getInstance()->setMessage("Executing " + command);
    process.setWorkingDirectory(dir.absolutePath());
    process.start(command);
}

QString RunSimulation::getGmx() const
{
    Settings settings;
    return settings.value(Settings::GMX_PATH).toString();
}

bool RunSimulation::execGrompp(
        const QString& mdpFile,
        const QString& inputStructure,
        const QString& topology,
        const QString& output,
        const QString& workingDirectory
        )
{
    QString command = getGmx() + " grompp";
    command += " -f " + mdpFile;
    command += " -c " + inputStructure;
    command += " -p " + topology;
    command += " -maxwarn 2 ";
    command += " -o " + output;

    qDebug() << "executing grompp" << command;
    QProcess grompp;
    grompp.setWorkingDirectory(workingDirectory);

    grompp.start(command);
    grompp.waitForFinished();

    const bool successful = grompp.exitCode() == 0;
    if (!successful)
    {
        qDebug() << "error executing grompp";
        StatusMessageSetter::getInstance()->setMessage("Could not execute " + command);
    }

    return successful;
}

}
