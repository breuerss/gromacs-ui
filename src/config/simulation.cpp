#include "simulation.h"
#include "temperaturecouplinggroup.h"
#include "../form/simulationsetupform.h"
#include <QDebug>
#include <QMetaProperty>
#include <cstring>
#include <memory>

namespace Config {

Simulation::Simulation()
{
  // Ugly hack to make SimulationType serializable as an integer
  // in QVariant
  static bool registered = false;
  if (!registered)
  {
    qRegisterMetaType<Type>("Simulation::Type");
    qRegisterMetaTypeStreamOperators<int>("Simulation::Type");
    qRegisterMetaType<PressureAlgorithm>("Simulation::PressureAlgorithm");
    qRegisterMetaTypeStreamOperators<int>("Simulation::PressureAlgorithm");
    qRegisterMetaType<PressureCouplingType>("Simulation::PressureCouplingType");
    qRegisterMetaTypeStreamOperators<int>("Simulation::PressureCouplingType");
    qRegisterMetaType<Algorithm>("Simulation::Algorithm");
    qRegisterMetaTypeStreamOperators<int>("Simulation::Algorithm");
    qRegisterMetaType<TemperatureAlgorithm>("Simulation::TemperatureAlgorithm");
    qRegisterMetaTypeStreamOperators<int>("Simulation::TemperatureAlgorithm");
    qRegisterMetaType<ElectrostaticAlgorithm>("Simulation::ElectrostaticAlgorithm");
    qRegisterMetaTypeStreamOperators<int>("Simulation::ElectrostaticAlgorithm");
    qRegisterMetaType<VdwAlgorithm>("Simulation::VdwAlgorithm");
    qRegisterMetaTypeStreamOperators<int>("Simulation::VdwAlgorithm");
    qRegisterMetaType<VdwModifier>("Simulation::VdwModifier");
    qRegisterMetaTypeStreamOperators<int>("Simulation::VdwModifier");
    registered = true;
  }

  addTemperatureCouplingGroup();
  connect(this, &Simulation::vdwAlgorithmChanged, this, &Simulation::pmeSettingsNeededChanged);
  connect(this, &Simulation::electrostaticAlgorithmChanged,
          this, &Simulation::pmeSettingsNeededChanged);
}

Simulation::~Simulation()
{
  disconnect(this, 0, 0, 0);
}

QString Simulation::getName() const
{
  return toString(simulationType);
}

QString Simulation::getTypeAsString() const
{
  return toString(simulationType, true);
}

QWidget* Simulation::getUI()
{
  return new SimulationSetupForm(shared_from_this());
}

bool Simulation::isMinimisation() const
{
  return algorithm == Simulation::Algorithm::SteepestDecent ||
    algorithm == Simulation::Algorithm::ConjugateGradient;
}

bool Simulation::pmeSettingsNeeded() const
{
  return electrostaticAlgorithm == ElectrostaticAlgorithm::PME ||
    electrostaticAlgorithm == ElectrostaticAlgorithm::P3MAD ||
    electrostaticAlgorithm == ElectrostaticAlgorithm::Ewald ||
    vdwAlgorithm == VdwAlgorithm::PME;
}

std::vector<std::shared_ptr<TemperatureCouplingGroup>>& Simulation::getTemperatureCouplingGroups()
{
  return temperatureCouplingGroups;
}

const std::vector<std::shared_ptr<TemperatureCouplingGroup>>& Simulation::getTemperatureCouplingGroups() const
{
  return temperatureCouplingGroups;
}

std::shared_ptr<TemperatureCouplingGroup> Simulation::addTemperatureCouplingGroup()
{
  qDebug() << "adding new temperature coupling group";
  auto group = std::make_shared<TemperatureCouplingGroup>();
  temperatureCouplingGroups.push_back(group);
  emit temperatureCouplingGroupAdded(group, temperatureCouplingGroups.size() - 1);
  return group;
}

void Simulation::removeTemperatureCouplingGroup(int at)
{
  auto couplingGroup = temperatureCouplingGroups[at];
  temperatureCouplingGroups.erase(temperatureCouplingGroups.begin() + at);
  emit temperatureCouplingGroupRemoved(couplingGroup, at);
}

const static auto simulationAlgorithmBimap = makeBimap<Simulation::Algorithm, QString>({
  { Simulation::Algorithm::None, "" },
  { Simulation::Algorithm::SteepestDecent, "steep" },
  { Simulation::Algorithm::ConjugateGradient, "cg" },
  { Simulation::Algorithm::LeapFrog, "md" },
  { Simulation::Algorithm::StochasticDynamics, "sd" },
});

QVariant simulationAlgorithmFrom(const QString& value)
{
  return QVariant::fromValue(simulationAlgorithmBimap.right.at(value));
}

QString toString(Simulation::Algorithm algorithm)
{
  return simulationAlgorithmBimap.left.at(algorithm);
}

const static auto pressureAlgorithmBimap = makeBimap<Simulation::PressureAlgorithm, QString>({
  { Simulation::PressureAlgorithm::None, "no" },
  { Simulation::PressureAlgorithm::Berendsen, "berendsen" },
  { Simulation::PressureAlgorithm::ParrinelloRahman, "Parrinello-Rahman" },
  { Simulation::PressureAlgorithm::Bussi, "C-rescale" },
});

QVariant pressureAlgorithmFrom(const QString& value)
{
  return QVariant::fromValue(pressureAlgorithmBimap.right.at(value));
}

QString toString(Simulation::PressureAlgorithm algorithm)
{
  return pressureAlgorithmBimap.left.at(algorithm);
}

QString toString(Simulation::Type type, bool shortVersion)
{
  const static QMap<Simulation::Type, QString> map = {
    { Simulation::Type::None, "None" },
    { Simulation::Type::Minimisation, "Minimisation" },
    { Simulation::Type::NVE, "Microcanonical (NVE)" },
    { Simulation::Type::NVT, "Canonical (NVT)" },
    { Simulation::Type::NPT, "Gibbs/Isobaric-isothermal (NPT)" },
  };

  const static QMap<Simulation::Type, QString> shortMap = {
    { Simulation::Type::None, "" },
    { Simulation::Type::Minimisation, "min" },
    { Simulation::Type::NVE, "nve" },
    { Simulation::Type::NVT, "nvt" },
    { Simulation::Type::NPT, "npt" },
  };

  if (shortVersion)
  {
    return shortMap[type];
  }
  else
  {
    return map[type];
  }
}

const static auto pressureCouplingTypeBimap =
makeBimap<Simulation::PressureCouplingType, QString>({
  { Simulation::PressureCouplingType::Isotropic, "isotropic" },
  { Simulation::PressureCouplingType::SemiIsoTropic, "semiisotropic" },
  { Simulation::PressureCouplingType::Anisotropic, "anisotropic" },
  { Simulation::PressureCouplingType::SurfaceTension, "surface-tension" },
});

QVariant pressureCouplingTypeFrom(const QString& value)
{
  return QVariant::fromValue(pressureCouplingTypeBimap.right.at(value));
}

QString toString(Simulation::PressureCouplingType type)
{
  return pressureCouplingTypeBimap.left.at(type);
}

QDataStream &operator<<(QDataStream &out, const Simulation &model)
{
  const Model::Serializable& tmp = model;
  out << tmp;

  const auto& groups = model.getTemperatureCouplingGroups();
  int noOfGroups = groups.size();
  out << noOfGroups;
  for (int groupIndex = 0; groupIndex < noOfGroups; groupIndex++)
  {
    out << *(groups[groupIndex]);
  }


  return out;
}

QDataStream &operator>>(QDataStream &in, Simulation& model)
{
  Model::Serializable& tmp = model;
  in >> tmp;

  int noOfGroups;
  in >> noOfGroups;

  auto& groups = model.getTemperatureCouplingGroups();
  groups.clear();
  for (int groupIndex = 0; groupIndex < noOfGroups; groupIndex++)
  {
    auto group = model.addTemperatureCouplingGroup();
    in >> (*group);
  }

  return in;
}

const static auto temperatureAlgorithmBimap =
makeBimap<Simulation::TemperatureAlgorithm, QString>({
  { Simulation::TemperatureAlgorithm::None, "no" },
  { Simulation::TemperatureAlgorithm::Berendsen, "berendsen" },
  { Simulation::TemperatureAlgorithm::NoseHoover, "nose-hoover" },
  { Simulation::TemperatureAlgorithm::Andersen, "andersen" },
  { Simulation::TemperatureAlgorithm::AndersenMassive, "andersen-massive" },
  { Simulation::TemperatureAlgorithm::VelocityRescale, "V-rescale" },
});

QVariant temperatureAlgorithmFrom(const QString& value)
{
  return QVariant::fromValue(temperatureAlgorithmBimap.right.at(value));
}

QString toString(Simulation::TemperatureAlgorithm algorithm)
{
  return temperatureAlgorithmBimap.left.at(algorithm);
}

const static auto electrostaticAlgorithmBimap =
makeBimap<Simulation::ElectrostaticAlgorithm, QString>({
  { Simulation::ElectrostaticAlgorithm::CutOff, "Cut-Off" },
  { Simulation::ElectrostaticAlgorithm::Ewald, "Ewald" },
  { Simulation::ElectrostaticAlgorithm::PME, "PME" },
  { Simulation::ElectrostaticAlgorithm::P3MAD, "P3M-AD" },
  { Simulation::ElectrostaticAlgorithm::ReactionField, "Reaction-Field" },
});

QVariant electrostaticAlgorithmFrom(const QString& value)
{
  return QVariant::fromValue(electrostaticAlgorithmBimap.right.at(value));
}

QString toString(Simulation::ElectrostaticAlgorithm algorithm)
{
  return electrostaticAlgorithmBimap.left.at(algorithm);
}

const static auto vdwAlgorithmBimap =
makeBimap<Simulation::VdwAlgorithm, QString>({
  { Simulation::VdwAlgorithm::CutOff, "Cut-Off" },
  { Simulation::VdwAlgorithm::PME, "PME" },
});

QVariant vdwAlgorithmFrom(const QString& value)
{
  return QVariant::fromValue(vdwAlgorithmBimap.right.at(value));
}

QString toString(Simulation::VdwAlgorithm algorithm)
{
  return vdwAlgorithmBimap.left.at(algorithm);
}

const static auto vdwModifierBimap =
makeBimap<Simulation::VdwModifier, QString>({
  { Simulation::VdwModifier::None, "None" },
  { Simulation::VdwModifier::PotentialShift, "Potential-Shift" },
  { Simulation::VdwModifier::ForceSwitch, "Force-Switch" },
  { Simulation::VdwModifier::PotentialSwitch, "Potential-Switch" },
});

QVariant vdwModifierFrom(const QString& value)
{
  return QVariant::fromValue(vdwModifierBimap.right.at(value));
}

QString toString(Simulation::VdwModifier algorithm)
{
  return vdwModifierBimap.left.at(algorithm);
}

}
