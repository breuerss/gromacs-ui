#ifndef RUNSIMULATION_H
#define RUNSIMULATION_H

#include "executor.h"
#include <memory>

namespace Model {
class Project;
}

namespace Command {

class RunSimulation : public Executor
{
public:
  explicit RunSimulation(std::shared_ptr<Model::Project> project, int stepIndex, QObject *parent = nullptr);
  void doExecute() override;

private:
  bool execGrompp(
    const QString& mdpFile,
    const QString& inputStructure,
    const QString& topology,
    const QString& output,
    const QString& workingDirectory
    );

  std::shared_ptr<Model::Project> project;
  int stepIndex;
};

}

#endif // RUNSIMULATION_H
