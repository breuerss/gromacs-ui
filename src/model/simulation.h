#ifndef STEP_H
#define STEP_H

#include "serializable.h"
#include "temperaturecouplinggroup.h"
#include <QString>
#include <QDataStream>
#include <memory>

namespace Model {

class Simulation : public Serializable
{
  Q_OBJECT
public:
  enum class Algorithm : int {
    None = 0,
    SteepestDecent,
    ConjugateGradient,
    LeapFrog,
    StochasticDynamics
  };
  Q_ENUM(Algorithm);

  enum class PressureAlgorithm : int {
    None = 0,
    Berendsen,
    ParrinelloRahman
  };
  Q_ENUM(PressureAlgorithm);

  enum class PressureCouplingType : int {
    Isotropic = 0,
    SemiIsoTropic,
    Anisotropic,
    SurfaceTension,
  };
  Q_ENUM(PressureCouplingType);

  enum class Type : int {
    None = 0,
    Minimisation,
    NVE,
    NVT,
    NPT
  };
  Q_ENUM(Type);

  enum class TemperatureAlgorithm : int {
    None = 0,
    Berendsen,
    NoseHoover,
    Andersen,
    AndersenMassive,
    VelocityRescale,
  };
  Q_ENUM(TemperatureAlgorithm);

  Simulation();
  QString getName() const;
  QString getDirectory() const;

  // temperature
  std::vector<std::shared_ptr<TemperatureCouplingGroup>>& getTemperatureCouplingGroups();
  Q_PROPERTY(Type simulationType MEMBER simulationType NOTIFY simulationTypeChanged);
  Q_PROPERTY(Algorithm algorithm MEMBER algorithm NOTIFY algorithmChanged);

  Q_PROPERTY(int numberOfSteps MEMBER numberOfSteps NOTIFY numberOfStepsChanged);
  Q_PROPERTY(double minimisationMaximumForce MEMBER minimisationMaximumForce NOTIFY minimisationMaximumForceChanged);
  Q_PROPERTY(double minimisationStepSize MEMBER minimisationStepSize NOTIFY minimisationStepSizeChanged);
  Q_PROPERTY(int energyOutputFrequency MEMBER energyOutputFrequency NOTIFY energyOutputFrequencyChanged);
  Q_PROPERTY(int positionOutputFrequency MEMBER positionOutputFrequency NOTIFY positionOutputFrequencyChanged);
  Q_PROPERTY(int compressedPositionOutputFrequency MEMBER compressedPositionOutputFrequency NOTIFY compressedPositionOutputFrequencyChanged);
  Q_PROPERTY(int velocityOutputFrequency MEMBER velocityOutputFrequency NOTIFY velocityOutputFrequencyChanged);
  Q_PROPERTY(int forceOutputFrequency MEMBER forceOutputFrequency NOTIFY forceOutputFrequencyChanged);
  Q_PROPERTY(int logOutputFrequency MEMBER logOutputFrequency NOTIFY logOutputFrequencyChanged);
  Q_PROPERTY(QString electrostaticAlgorithm MEMBER electrostaticAlgorithm NOTIFY electrostaticAlgorithmChanged);
  Q_PROPERTY(double electrostaticCutoffRadius MEMBER electrostaticCutoffRadius NOTIFY electrostaticCutoffRadiusChanged);
  Q_PROPERTY(double fourierSpacing MEMBER fourierSpacing NOTIFY fourierSpacingChanged);
  Q_PROPERTY(double vdwCutoffRadius MEMBER vdwCutoffRadius NOTIFY vdwCutoffRadiusChanged);

  Q_PROPERTY(Simulation::PressureAlgorithm pressureAlgorithm MEMBER pressureAlgorithm NOTIFY pressureAlgorithmChanged);
  Q_PROPERTY(double pressure MEMBER pressure NOTIFY pressureChanged);
  Q_PROPERTY(double pressureUpdateInterval MEMBER pressureUpdateInterval NOTIFY pressureUpdateIntervalChanged);
  Q_PROPERTY(Simulation::PressureCouplingType pressureCouplingType MEMBER pressureCouplingType NOTIFY pressureCouplingTypeChanged);

  Q_PROPERTY(TemperatureAlgorithm temperatureAlgorithm
             MEMBER temperatureAlgorithm NOTIFY temperatureAlgorithmChanged);

signals:
  void nameChanged();
  void directoryChanged();

  void simulationTypeChanged(Type);
  void algorithmChanged(Algorithm);

  void numberOfStepsChanged(int);
  void minimisationMaximumForceChanged(double);
  void minimisationStepSizeChanged(double);

  void energyOutputFrequencyChanged(int);
  void positionOutputFrequencyChanged(int);
  void compressedPositionOutputFrequencyChanged(int);
  void logOutputFrequencyChanged(int);
  void velocityOutputFrequencyChanged(int);
  void forceOutputFrequencyChanged(int);

  void electrostaticAlgorithmChanged(QString);
  void electrostaticCutoffRadiusChanged(double);
  void fourierSpacingChanged(double);
  void vdwCutoffRadiusChanged(double);

  void pressureAlgorithmChanged(PressureAlgorithm);
  void pressureChanged(double);
  void pressureUpdateIntervalChanged(double);
  void pressureCouplingTypeChanged(PressureCouplingType);

  void temperatureAlgorithmChanged(TemperatureAlgorithm);
  void temperatureCouplingGroupAdded(std::shared_ptr<TemperatureCouplingGroup>, int at);
  void temperatureCouplingGroupRemoved(std::shared_ptr<TemperatureCouplingGroup>, int at);

public slots:
  const std::vector<std::shared_ptr<TemperatureCouplingGroup>>& getTemperatureCouplingGroups() const;
  std::shared_ptr<TemperatureCouplingGroup> addTemperatureCouplingGroup();
  void removeTemperatureCouplingGroup(int index);

private:
  Type simulationType = Type::None;
  Algorithm algorithm = Algorithm::None;

  int numberOfSteps = -1;

  // minimsation options
  double minimisationMaximumForce = 1000.0;
  double minimisationStepSize = 0.01;

  // output frequencies
  int energyOutputFrequency = 500;
  int positionOutputFrequency = 0;
  int compressedPositionOutputFrequency = 1000;
  int logOutputFrequency = 1000;
  int velocityOutputFrequency = 0;
  int forceOutputFrequency = 0;

  // electrostatics
  QString electrostaticAlgorithm = "PME";
  double electrostaticCutoffRadius = 1.0;
  double fourierSpacing = 0.125;

  // vdw
  double vdwCutoffRadius = 1.0;

  // pressure
  PressureAlgorithm pressureAlgorithm = PressureAlgorithm::None;
  double pressure = 1;
  double pressureUpdateInterval = 1;
  PressureCouplingType pressureCouplingType = PressureCouplingType::Isotropic;

  // temperature
  TemperatureAlgorithm temperatureAlgorithm = TemperatureAlgorithm::None;
  std::vector<std::shared_ptr<TemperatureCouplingGroup>> temperatureCouplingGroups;
};

QString toString(Simulation::Algorithm algorithm);
QString toString(Simulation::PressureAlgorithm alogrithm);
QString toString(Simulation::PressureCouplingType type);
QString toString(Simulation::TemperatureAlgorithm algorithm);
QString toString(Simulation::Type type, bool shortVersion = false);

QDataStream &operator<<(QDataStream &out, const Simulation &model);
QDataStream &operator>>(QDataStream &in, Simulation& model);

}

#endif // STEP_H
