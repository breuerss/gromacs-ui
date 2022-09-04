#ifndef PIPELINE_SIMULATION_COMMAND_H
#define PIPELINE_SIMULATION_COMMAND_H

#include "../../command/executor.h"
#include <memory>
#include <QFileSystemWatcher>
#include <qwidget.h>

namespace Model {
class Project;
}

class SimulationStatusChecker;

namespace Pipeline { namespace Simulation {

class Command : public ::Command::Executor
{
public:
  explicit Command(std::shared_ptr<Model::Project> project);
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
  QString getInputFileName() const;
};

} }

#endif
