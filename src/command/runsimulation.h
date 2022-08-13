#ifndef RUNSIMULATION_H
#define RUNSIMULATION_H

#include "executor.h"
#include <memory>
#include <QFileSystemWatcher>

namespace Model {
class Project;
class Simulation;
}

class SimulationStatusChecker;

namespace Command {

class RunSimulation : public Executor
{
public:
  explicit RunSimulation(
    std::shared_ptr<Model::Project> project,
    std::shared_ptr<Model::Simulation> simulation,
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
  std::shared_ptr<Model::Simulation> simulation;
  std::shared_ptr<SimulationStatusChecker> simulationChecker;
  QFileSystemWatcher progressChecker;
  void checkProgress();
};

}

#endif // RUNSIMULATION_H
