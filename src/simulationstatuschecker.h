#ifndef SIMULATIONSTATUSCHECKER_H
#define SIMULATIONSTATUSCHECKER_H

#include <QObject>
#include <memory>

namespace Model {
class Project;
class Simulation;
}

class SimulationStatusChecker : public QObject
{
  Q_OBJECT
public:
  explicit SimulationStatusChecker(
    std::shared_ptr<Model::Project> project,
    std::shared_ptr<Model::Simulation> simulation,
    QObject *parent = nullptr);

  bool hasData() const;
  bool hasTrajectory() const;
  bool hasCoordinates() const;
  bool hasLog() const;
  bool hasMdp() const;
  QString getTrajectoryPath() const;
  QString getCoordinatesPath() const;
  QString getLogPath() const;
  QString getMdpPath() const;

signals:
private:
  QString getBasePath() const;

  std::shared_ptr<Model::Project> project;
  std::shared_ptr<Model::Simulation> simulation;

};

#endif // SIMULATIONSTATUSCHECKER_H
