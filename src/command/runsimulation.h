#ifndef RUNSIMULATION_H
#define RUNSIMULATION_H

#include "executor.h"
#include <memory>
#include <QFileSystemWatcher>
#include "../pipeline/step.h"

namespace Config {
class Simulation;
}
namespace Model {
class Project;
}

class SimulationStatusChecker;

namespace Command {

class RunSimulation : public Executor, public Pipeline::Step
{
public:
  explicit RunSimulation(
    std::shared_ptr<Model::Project> project,
    QObject *parent = nullptr);
  void doExecute() override;
  QString getName() const override;

private:
  bool execGrompp(
    const QString& mdpFile,
    const QString& inputStructure,
    const QString& topology,
    const QString& output,
    const QString& workingDirectory
    );

  std::shared_ptr<Model::Project> project;
  std::shared_ptr<SimulationStatusChecker> simulationChecker;
  QFileSystemWatcher progressChecker;
  void checkProgress();
};

}

#endif // RUNSIMULATION_H
