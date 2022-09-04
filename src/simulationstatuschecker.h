#ifndef SIMULATIONSTATUSCHECKER_H
#define SIMULATIONSTATUSCHECKER_H

#include <QObject>
#include <QList>
#include <memory>

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

  bool hasData() const;
  bool hasTrajectory() const;
  bool hasCoordinates() const;
  bool hasInputCoordinates() const;
  bool hasLog() const;
  bool hasMdp() const;
  bool hasTpr() const;
  QString getTrajectoryPath() const;
  QString getSmoothTrajectoryPath() const;
  QString getCoordinatesPath() const;
  QString getInputCoordinatesPath() const;
  QString getLogPath() const;
  QString getMdpPath() const;
  QString getTprPath() const;

  float getProgress() const;
  QList<float> getProgressValues() const;

private:
  QString getBasePath() const;

  std::shared_ptr<Model::Project> project;
  const Pipeline::Simulation::Configuration* simulation;
};

#endif // SIMULATIONSTATUSCHECKER_H
