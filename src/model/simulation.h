#ifndef STEP_H
#define STEP_H

#include "serializable.h"
#include "temperaturecouplinggroup.h"
#include <QString>
#include <QDataStream>
#include <memory>
#include "../misc/bimaphelper.h"

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
    ParrinelloRahman,
    Bussi,
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

  enum class ElectrostaticAlgorithm : int {
    CutOff = 0,
    Ewald,
    PME,
    P3MAD,
    ReactionField,
  };
  Q_ENUM(ElectrostaticAlgorithm);

  enum class VdwAlgorithm : int {
    CutOff = 0,
    PME,
  };
  Q_ENUM(VdwAlgorithm);

  enum class VdwModifier : int {
    None = 0,
    PotentialShift,
    ForceSwitch,
    PotentialSwitch,
  };
  Q_ENUM(VdwModifier);

  Simulation();
  ~Simulation();
  QString getName() const;
  QString getTypeAsString() const;

  std::shared_ptr<Simulation> getPreviousStep() const;
  void setPreviousStep(std::shared_ptr<Simulation> newPreviousStep);

  bool isMinimisation() const;
  bool pmeSettingsNeeded() const;

  // temperature
  std::vector<std::shared_ptr<TemperatureCouplingGroup>>& getTemperatureCouplingGroups();
  Q_PROPERTY(Type simulationType MEMBER simulationType NOTIFY simulationTypeChanged);
  Q_PROPERTY(Algorithm algorithm MEMBER algorithm NOTIFY algorithmChanged);

  Q_PROPERTY(long long numberOfSteps MEMBER numberOfSteps NOTIFY numberOfStepsChanged);
  Q_PROPERTY(float timeStep MEMBER timeStep NOTIFY timeStepChanged);
  Q_PROPERTY(float minimisationMaximumForce MEMBER minimisationMaximumForce NOTIFY minimisationMaximumForceChanged);
  Q_PROPERTY(float minimisationStepSize MEMBER minimisationStepSize NOTIFY minimisationStepSizeChanged);

  // output control
  Q_PROPERTY(unsigned int energyOutputFrequency MEMBER energyOutputFrequency NOTIFY energyOutputFrequencyChanged);
  Q_PROPERTY(unsigned int positionOutputFrequency MEMBER positionOutputFrequency NOTIFY positionOutputFrequencyChanged);
  Q_PROPERTY(unsigned int compressedPositionOutputFrequency MEMBER compressedPositionOutputFrequency NOTIFY compressedPositionOutputFrequencyChanged);
  Q_PROPERTY(unsigned int velocityOutputFrequency MEMBER velocityOutputFrequency NOTIFY velocityOutputFrequencyChanged);
  Q_PROPERTY(unsigned int forceOutputFrequency MEMBER forceOutputFrequency NOTIFY forceOutputFrequencyChanged);
  Q_PROPERTY(unsigned int logOutputFrequency MEMBER logOutputFrequency NOTIFY logOutputFrequencyChanged);

  // electrostatics
  Q_PROPERTY(ElectrostaticAlgorithm electrostaticAlgorithm MEMBER electrostaticAlgorithm NOTIFY electrostaticAlgorithmChanged);
  Q_PROPERTY(float electrostaticCutoffRadius MEMBER electrostaticCutoffRadius NOTIFY electrostaticCutoffRadiusChanged);
  Q_PROPERTY(double electrostaticEwaldRtol MEMBER electrostaticEwaldRtol NOTIFY electrostaticEwaldRtolChanged);

  // PME
  Q_PROPERTY(float fourierSpacing MEMBER fourierSpacing NOTIFY fourierSpacingChanged);
  Q_PROPERTY(float pmeOrder MEMBER pmeOrder NOTIFY pmeOrderChanged);

  // VdW
  Q_PROPERTY(VdwAlgorithm vdwAlgorithm MEMBER vdwAlgorithm NOTIFY vdwAlgorithmChanged);
  Q_PROPERTY(VdwModifier vdwModifier MEMBER vdwModifier NOTIFY vdwModifierChanged);
  Q_PROPERTY(float vdwCutoffRadius MEMBER vdwCutoffRadius NOTIFY vdwCutoffRadiusChanged);
  Q_PROPERTY(double vdwEwaldRtol MEMBER vdwEwaldRtol NOTIFY vdwEwaldRtolChanged);

  Q_PROPERTY(Simulation::PressureAlgorithm pressureAlgorithm MEMBER pressureAlgorithm NOTIFY pressureAlgorithmChanged);
  Q_PROPERTY(float pressure MEMBER pressure NOTIFY pressureChanged);
  Q_PROPERTY(float pressureUpdateInterval MEMBER pressureUpdateInterval NOTIFY pressureUpdateIntervalChanged);
  Q_PROPERTY(Simulation::PressureCouplingType pressureCouplingType MEMBER pressureCouplingType NOTIFY pressureCouplingTypeChanged);
  Q_PROPERTY(double compressibility MEMBER compressibility NOTIFY compressibilityChanged);

  Q_PROPERTY(bool resume MEMBER resume NOTIFY resumeChanged);

  Q_PROPERTY(TemperatureAlgorithm temperatureAlgorithm
             MEMBER temperatureAlgorithm NOTIFY temperatureAlgorithmChanged);

signals:
  void nameChanged();
  void directoryChanged();

  void simulationTypeChanged(Type);
  void algorithmChanged(Algorithm);

  void numberOfStepsChanged(long long);
  void timeStepChanged(float);
  void minimisationMaximumForceChanged(float);
  void minimisationStepSizeChanged(float);

  void energyOutputFrequencyChanged(unsigned int);
  void positionOutputFrequencyChanged(unsigned int);
  void compressedPositionOutputFrequencyChanged(unsigned int);
  void logOutputFrequencyChanged(unsigned int);
  void velocityOutputFrequencyChanged(unsigned int);
  void forceOutputFrequencyChanged(unsigned int);

  void electrostaticAlgorithmChanged(ElectrostaticAlgorithm);
  void electrostaticCutoffRadiusChanged(float);
  void electrostaticEwaldRtolChanged(float);


  void fourierSpacingChanged(float);
  void pmeOrderChanged(unsigned int);

  void vdwCutoffRadiusChanged(float);
  void vdwAlgorithmChanged(VdwAlgorithm);
  void vdwModifierChanged(VdwModifier);
  void vdwEwaldRtolChanged(double);

  void pressureAlgorithmChanged(PressureAlgorithm);
  void pressureChanged(float);
  void pressureUpdateIntervalChanged(float);
  void pressureCouplingTypeChanged(PressureCouplingType);
  void compressibilityChanged(double);

  void resumeChanged(bool newResume);

  void temperatureAlgorithmChanged(TemperatureAlgorithm);
  void temperatureCouplingGroupAdded(std::shared_ptr<TemperatureCouplingGroup>, int at);
  void temperatureCouplingGroupRemoved(std::shared_ptr<TemperatureCouplingGroup>, int at);

  void pmeSettingsNeededChanged();

public slots:
  const std::vector<std::shared_ptr<TemperatureCouplingGroup>>& getTemperatureCouplingGroups() const;
  std::shared_ptr<TemperatureCouplingGroup> addTemperatureCouplingGroup();
  void removeTemperatureCouplingGroup(int index);

private:
  Type simulationType = Type::None;
  Algorithm algorithm = Algorithm::None;

  long long numberOfSteps = -1;
  float timeStep = 2;

  // minimsation options
  float minimisationMaximumForce = 1000.0;
  float minimisationStepSize = 0.01;

  // output frequencies
  unsigned int energyOutputFrequency = 500;
  unsigned int positionOutputFrequency = 0;
  unsigned int compressedPositionOutputFrequency = 1000;
  unsigned int logOutputFrequency = 1000;
  unsigned int velocityOutputFrequency = 0;
  unsigned int forceOutputFrequency = 0;

  // electrostatics
  ElectrostaticAlgorithm electrostaticAlgorithm = ElectrostaticAlgorithm::PME;
  float electrostaticCutoffRadius = 1.0;
  double electrostaticEwaldRtol = 0.00001;

  // PME
  float fourierSpacing = 0.125;
  unsigned int pmeOrder = 4;

  // vdw
  VdwAlgorithm vdwAlgorithm = VdwAlgorithm::PME;
  VdwModifier vdwModifier = VdwModifier::None;
  float vdwCutoffRadius = 1.0;
  double vdwEwaldRtol = 0.001;

  // pressure
  PressureAlgorithm pressureAlgorithm = PressureAlgorithm::None;
  float pressure = 1;
  float pressureUpdateInterval = 1;
  double compressibility = 0.000045;
  PressureCouplingType pressureCouplingType = PressureCouplingType::Isotropic;

  // temperature
  TemperatureAlgorithm temperatureAlgorithm = TemperatureAlgorithm::None;
  std::vector<std::shared_ptr<TemperatureCouplingGroup>> temperatureCouplingGroups;

  std::shared_ptr<Simulation> previousStep;

  bool resume = true;
};

QVariant simulationAlgorithmFrom(const QString& value);
QString toString(Simulation::Algorithm algorithm);
QVariant pressureAlgorithmFrom(const QString& value);
QString toString(Simulation::PressureAlgorithm alogrithm);
QVariant pressureCouplingTypeFrom(const QString& value);
QString toString(Simulation::PressureCouplingType type);
QVariant temperatureAlgorithmFrom(const QString& value);
QString toString(Simulation::TemperatureAlgorithm algorithm);
QString toString(Simulation::Type type, bool shortVersion = false);
QVariant electrostaticAlgorithmFrom(const QString& value);
QString toString(Simulation::ElectrostaticAlgorithm algorithm);
QVariant vdwAlgorithmFrom(const QString& value);
QString toString(Simulation::VdwAlgorithm algorithm);
QVariant vdwModifierFrom(const QString& value);
QString toString(Simulation::VdwModifier algorithm);

QDataStream &operator<<(QDataStream &out, const Simulation &model);
QDataStream &operator>>(QDataStream &in, Simulation& model);

}

#endif // STEP_H
