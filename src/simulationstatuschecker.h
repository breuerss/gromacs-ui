#ifndef SIMULATIONSTATUSCHECKER_H
#define SIMULATIONSTATUSCHECKER_H

#include <QObject>
#include <QList>
#include "pipeline/simulation/filenamegenerator.h"

namespace Pipeline { namespace Simulation {
class Configuration;
} }

namespace Model {
class Project;
}

class SimulationStatusChecker : public QObject
{
  Q_OBJECT
public:
  explicit SimulationStatusChecker(
    std::shared_ptr<Model::Project> project,
    const Pipeline::Simulation::Configuration* simulation,
    QObject *parent = nullptr);

  float getProgress() const;
  QList<float> getProgressValues() const;

private:
  std::shared_ptr<Model::Project> project;
  const Pipeline::Simulation::Configuration* simulation;
  std::unique_ptr<Pipeline::Simulation::FileNameGenerator> fileNameGenerator;
};

#endif // SIMULATIONSTATUSCHECKER_H
