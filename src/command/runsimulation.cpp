#include "runsimulation.h"
#include "../gromacsconfigfilegenerator.h"
#include "../statusmessagesetter.h"
#include "../model/systemsetup.h"
#include "../model/project.h"
#include "../model/simulation.h"
#include "../appprovider.h"
#include "../logforwarder.h"
#include "../simulationstatuschecker.h"

#include <QDebug>
#include <QDir>

namespace Command {

RunSimulation::RunSimulation(
  std::shared_ptr<Model::Project> project,
  int stepIndex,
  QObject *parent)
  : Executor(parent)
    , project(project)
    , stepIndex(stepIndex)
{
}

void RunSimulation::exec()
{
  qDebug() << "Exec simulation";
  QString gmx = AppProvider::get("gmx");
  if (gmx.isEmpty())
  {
    QString message("Path to 'gmx' command is not set.");
    StatusMessageSetter::getInstance()->setMessage(message);
    return;
  }

  const auto& steps = project->getSteps();
  std::shared_ptr<Model::Simulation> currentSim = steps[stepIndex];

  SimulationStatusChecker currentSimChecker(project, currentSim);
  QString mdpFile = currentSimChecker.getMdpPath();
  QFileInfo fi(mdpFile);
  QDir dir(fi.absolutePath());
  dir.mkpath(".");

  GromacsConfigFileGenerator::generate(currentSim, mdpFile);

  QString inputStructure = project->getSystemSetup()->getNeutralisedStructureFile();
  QFileInfo systemPath(inputStructure);
  if (stepIndex > 0)
  {
    auto prevStep = steps[stepIndex - 1];
    SimulationStatusChecker prevSimChecker(project, prevStep);
    inputStructure = prevSimChecker.getCoordinatesPath();
  }

  if (!execGrompp(
      mdpFile,
      inputStructure,
      systemPath.absolutePath() + "/topol.top",
      currentSimChecker.getTprPath(),
      dir.absolutePath()
      ))
  {
    return;
  }

  const QString command = gmx + " mdrun -v -deffnm " + currentSim->getTypeAsString();
  qDebug() << "executing" << command;
  StatusMessageSetter::getInstance()->setMessage("Executing " + command);
  process.setWorkingDirectory(dir.absolutePath());
  process.start(command);
}

bool RunSimulation::execGrompp(
  const QString& mdpFile,
  const QString& inputStructure,
  const QString& topology,
  const QString& output,
  const QString& workingDirectory
  )
{
  QString command = AppProvider::get("gmx") + " grompp";
  command += " -f " + mdpFile;
  command += " -c " + inputStructure;
  command += " -p " + topology;
  command += " -maxwarn 2 ";
  command += " -o " + output;

  qDebug() << "executing grompp" << command;
  QProcess grompp;
  LogForwarder::getInstance()->listenTo(&grompp);
  grompp.setWorkingDirectory(workingDirectory);

  grompp.start(command);
  grompp.waitForFinished();

  const bool successful = grompp.exitCode() == 0;
  if (!successful)
  {
    qDebug() << "error executing grompp";
    StatusMessageSetter::getInstance()->setMessage("Could not execute " + command);
  }

  LogForwarder::getInstance()->detach(&grompp);
  return successful;
}

}
