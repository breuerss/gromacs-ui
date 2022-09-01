#ifndef RUNSIMULATION_H
#define RUNSIMULATION_H

#include "executor.h"
#include <memory>
#include <QFileSystemWatcher>
#include <qwidget.h>
#include "../pipeline/step.h"

namespace Model {
class Project;
}

class SimulationStatusChecker;

namespace Command {

class RunSimulation : public Executor, public std::enable_shared_from_this<RunSimulation>
{
public:
  explicit RunSimulation(std::shared_ptr<Model::Project> project);
  void doExecute() override;
  bool canExecute() const override;
  QWidget* getStatusUi() override;

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
