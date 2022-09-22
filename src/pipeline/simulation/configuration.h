#ifndef PIPELINE_SIMULATION_CONFIGURATION_H
#define PIPELINE_SIMULATION_CONFIGURATION_H

#include "temperaturecouplinggroup.h"
#include "../../config/configuration.h"
#include "../../misc/bimaphelper.h"
#include <QString>
#include <memory>

namespace Pipeline { namespace Simulation {

class Configuration
: public Config::Configuration
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

  Configuration();
  ~Configuration();
  QString getName() const;
  QString getTypeAsString() const;

  QWidget* getUI() override;

  bool isMinimisation() const;
  bool pmeSettingsNeeded() const;

  // temperature
  std::vector<std::shared_ptr<TemperatureCouplingGroup>>& getTemperatureCouplingGroups();
  Q_PROPERTY(Type simulationType MEMBER simulationType NOTIFY simulationTypeChanged);
  Q_PROPERTY(Algorithm algorithm MEMBER algorithm NOTIFY algorithmChanged);

  Q_PROPERTY(double numberOfSteps MEMBER numberOfSteps NOTIFY numberOfStepsChanged);
  Q_PROPERTY(double timeStep MEMBER timeStep NOTIFY timeStepChanged);
  Q_PROPERTY(double minimisationMaximumForce MEMBER minimisationMaximumForce NOTIFY minimisationMaximumForceChanged);
  Q_PROPERTY(double minimisationStepSize MEMBER minimisationStepSize NOTIFY minimisationStepSizeChanged);

  // output control
  Q_PROPERTY(int energyOutputFrequency MEMBER energyOutputFrequency NOTIFY energyOutputFrequencyChanged);
  Q_PROPERTY(int positionOutputFrequency MEMBER positionOutputFrequency NOTIFY positionOutputFrequencyChanged);
  Q_PROPERTY(int compressedPositionOutputFrequency MEMBER compressedPositionOutputFrequency NOTIFY compressedPositionOutputFrequencyChanged);
  Q_PROPERTY(int velocityOutputFrequency MEMBER velocityOutputFrequency NOTIFY velocityOutputFrequencyChanged);
  Q_PROPERTY(int forceOutputFrequency MEMBER forceOutputFrequency NOTIFY forceOutputFrequencyChanged);
  Q_PROPERTY(int logOutputFrequency MEMBER logOutputFrequency NOTIFY logOutputFrequencyChanged);

  // electrostatics
  Q_PROPERTY(ElectrostaticAlgorithm electrostaticAlgorithm MEMBER electrostaticAlgorithm NOTIFY electrostaticAlgorithmChanged);
  Q_PROPERTY(double electrostaticCutoffRadius MEMBER electrostaticCutoffRadius NOTIFY electrostaticCutoffRadiusChanged);
  Q_PROPERTY(double electrostaticEwaldRtol MEMBER electrostaticEwaldRtol NOTIFY electrostaticEwaldRtolChanged);

  // PME
  Q_PROPERTY(double fourierSpacing MEMBER fourierSpacing NOTIFY fourierSpacingChanged);
  Q_PROPERTY(int pmeOrder MEMBER pmeOrder NOTIFY pmeOrderChanged);

  // VdW
  Q_PROPERTY(VdwAlgorithm vdwAlgorithm MEMBER vdwAlgorithm NOTIFY vdwAlgorithmChanged);
  Q_PROPERTY(VdwModifier vdwModifier MEMBER vdwModifier NOTIFY vdwModifierChanged);
  Q_PROPERTY(double vdwCutoffRadius MEMBER vdwCutoffRadius NOTIFY vdwCutoffRadiusChanged);
  Q_PROPERTY(double vdwEwaldRtol MEMBER vdwEwaldRtol NOTIFY vdwEwaldRtolChanged);

  Q_PROPERTY(PressureAlgorithm pressureAlgorithm MEMBER pressureAlgorithm NOTIFY pressureAlgorithmChanged);
  Q_PROPERTY(double pressure MEMBER pressure NOTIFY pressureChanged);
  Q_PROPERTY(double pressureUpdateInterval MEMBER pressureUpdateInterval NOTIFY pressureUpdateIntervalChanged);
  Q_PROPERTY(PressureCouplingType pressureCouplingType MEMBER pressureCouplingType NOTIFY pressureCouplingTypeChanged);
  Q_PROPERTY(double compressibility MEMBER compressibility NOTIFY compressibilityChanged);

  Q_PROPERTY(bool resume MEMBER resume NOTIFY resumeChanged);

  Q_PROPERTY(TemperatureAlgorithm temperatureAlgorithm
             MEMBER temperatureAlgorithm NOTIFY temperatureAlgorithmChanged);

  QString toString() override;

signals:
  void nameChanged();
  void directoryChanged();

  void simulationTypeChanged(Type);
  void algorithmChanged(Algorithm);

  void numberOfStepsChanged(double);
  void timeStepChanged(double);
  void minimisationMaximumForceChanged(double);
  void minimisationStepSizeChanged(double);

  void energyOutputFrequencyChanged(int);
  void positionOutputFrequencyChanged(int);
  void compressedPositionOutputFrequencyChanged(int);
  void logOutputFrequencyChanged(int);
  void velocityOutputFrequencyChanged(int);
  void forceOutputFrequencyChanged(int);

  void electrostaticAlgorithmChanged(ElectrostaticAlgorithm);
  void electrostaticCutoffRadiusChanged(double);
  void electrostaticEwaldRtolChanged(double);


  void fourierSpacingChanged(double);
  void pmeOrderChanged(int);

  void vdwCutoffRadiusChanged(double);
  void vdwAlgorithmChanged(VdwAlgorithm);
  void vdwModifierChanged(VdwModifier);
  void vdwEwaldRtolChanged(double);

  void pressureAlgorithmChanged(PressureAlgorithm);
  void pressureChanged(double);
  void pressureUpdateIntervalChanged(double);
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

  double numberOfSteps = -1;
  double timeStep = 2;

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
  ElectrostaticAlgorithm electrostaticAlgorithm = ElectrostaticAlgorithm::PME;
  double electrostaticCutoffRadius = 1.0;
  double electrostaticEwaldRtol = 0.00001;

  // PME
  double fourierSpacing = 0.125;
  int pmeOrder = 4;

  // vdw
  VdwAlgorithm vdwAlgorithm = VdwAlgorithm::PME;
  VdwModifier vdwModifier = VdwModifier::None;
  double vdwCutoffRadius = 1.0;
  double vdwEwaldRtol = 0.001;

  // pressure
  PressureAlgorithm pressureAlgorithm = PressureAlgorithm::None;
  double pressure = 1;
  double pressureUpdateInterval = 1;
  double compressibility = 0.000045;
  PressureCouplingType pressureCouplingType = PressureCouplingType::Isotropic;

  // temperature
  TemperatureAlgorithm temperatureAlgorithm = TemperatureAlgorithm::None;
  std::vector<std::shared_ptr<TemperatureCouplingGroup>> temperatureCouplingGroups;

  bool resume = true;
};

QVariant simulationAlgorithmFrom(const QString& value);
QString toString(Configuration::Algorithm algorithm);
QVariant pressureAlgorithmFrom(const QString& value);
QString toString(Configuration::PressureAlgorithm alogrithm);
QVariant pressureCouplingTypeFrom(const QString& value);
QString toString(Configuration::PressureCouplingType type);
QVariant temperatureAlgorithmFrom(const QString& value);
QString toString(Configuration::TemperatureAlgorithm algorithm);
QString toString(Configuration::Type type, bool shortVersion = false);
QVariant electrostaticAlgorithmFrom(const QString& value);
QString toString(Configuration::ElectrostaticAlgorithm algorithm);
QVariant vdwAlgorithmFrom(const QString& value);
QString toString(Configuration::VdwAlgorithm algorithm);
QVariant vdwModifierFrom(const QString& value);
QString toString(Configuration::VdwModifier algorithm);

} }

#endif
