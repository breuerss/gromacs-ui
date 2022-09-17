#include "configuration.h"
#include "temperaturecouplinggroup.h"
#include "../../../ui/simulationsetupform.h"
#include <QDebug>
#include <QMetaProperty>
#include <cstring>
#include <memory>

namespace Pipeline { namespace Simulation {

Configuration::Configuration()
{
  // Ugly hack to make SimulationType serializable as an integer
  // in QVariant
  static bool registered = false;
  if (!registered)
  {
    qRegisterMetaType<Type>("Configuration::Type");
    qRegisterMetaTypeStreamOperators<int>("Configuration::Type");
    qRegisterMetaType<PressureAlgorithm>("Configuration::PressureAlgorithm");
    qRegisterMetaTypeStreamOperators<int>("Configuration::PressureAlgorithm");
    qRegisterMetaType<PressureCouplingType>("Configuration::PressureCouplingType");
    qRegisterMetaTypeStreamOperators<int>("Configuration::PressureCouplingType");
    qRegisterMetaType<Algorithm>("Configuration::Algorithm");
    qRegisterMetaTypeStreamOperators<int>("Configuration::Algorithm");
    qRegisterMetaType<TemperatureAlgorithm>("Configuration::TemperatureAlgorithm");
    qRegisterMetaTypeStreamOperators<int>("Configuration::TemperatureAlgorithm");
    qRegisterMetaType<ElectrostaticAlgorithm>("Configuration::ElectrostaticAlgorithm");
    qRegisterMetaTypeStreamOperators<int>("Configuration::ElectrostaticAlgorithm");
    qRegisterMetaType<VdwAlgorithm>("Configuration::VdwAlgorithm");
    qRegisterMetaTypeStreamOperators<int>("Configuration::VdwAlgorithm");
    qRegisterMetaType<VdwModifier>("Configuration::VdwModifier");
    qRegisterMetaTypeStreamOperators<int>("Configuration::VdwModifier");
    registered = true;
  }

  addTemperatureCouplingGroup();
  connect(this, &Configuration::vdwAlgorithmChanged, this, &Configuration::pmeSettingsNeededChanged);
  connect(this, &Configuration::electrostaticAlgorithmChanged,
          this, &Configuration::pmeSettingsNeededChanged);

  connectAllSignals();
}

Configuration::~Configuration()
{
  disconnect(this, 0, 0, 0);
}

QString Configuration::getName() const
{
  return toString(simulationType);
}

QString Configuration::getTypeAsString() const
{
  return toString(simulationType, true);
}

QWidget* Configuration::getUI()
{
  return new SimulationSetupForm(this);
}

bool Configuration::isMinimisation() const
{
  return algorithm == Configuration::Algorithm::SteepestDecent ||
    algorithm == Configuration::Algorithm::ConjugateGradient;
}

bool Configuration::pmeSettingsNeeded() const
{
  return electrostaticAlgorithm == ElectrostaticAlgorithm::PME ||
    electrostaticAlgorithm == ElectrostaticAlgorithm::P3MAD ||
    electrostaticAlgorithm == ElectrostaticAlgorithm::Ewald ||
    vdwAlgorithm == VdwAlgorithm::PME;
}

std::vector<std::shared_ptr<TemperatureCouplingGroup>>& Configuration::getTemperatureCouplingGroups()
{
  return temperatureCouplingGroups;
}

const std::vector<std::shared_ptr<TemperatureCouplingGroup>>& Configuration::getTemperatureCouplingGroups() const
{
  return temperatureCouplingGroups;
}

std::shared_ptr<TemperatureCouplingGroup> Configuration::addTemperatureCouplingGroup()
{
  qDebug() << "adding new temperature coupling group";
  auto group = std::make_shared<TemperatureCouplingGroup>();
  temperatureCouplingGroups.push_back(group);
  emit temperatureCouplingGroupAdded(group, temperatureCouplingGroups.size() - 1);
  return group;
}

void Configuration::removeTemperatureCouplingGroup(int at)
{
  auto couplingGroup = temperatureCouplingGroups[at];
  temperatureCouplingGroups.erase(temperatureCouplingGroups.begin() + at);
  emit temperatureCouplingGroupRemoved(couplingGroup, at);
}

const static auto simulationAlgorithmBimap = makeBimap<Configuration::Algorithm, QString>({
  { Configuration::Algorithm::None, "" },
  { Configuration::Algorithm::SteepestDecent, "steep" },
  { Configuration::Algorithm::ConjugateGradient, "cg" },
  { Configuration::Algorithm::LeapFrog, "md" },
  { Configuration::Algorithm::StochasticDynamics, "sd" },
});

QVariant simulationAlgorithmFrom(const QString& value)
{
  return QVariant::fromValue(simulationAlgorithmBimap.right.at(value));
}

QString toString(Configuration::Algorithm algorithm)
{
  return simulationAlgorithmBimap.left.at(algorithm);
}

const static auto pressureAlgorithmBimap = makeBimap<Configuration::PressureAlgorithm, QString>({
  { Configuration::PressureAlgorithm::None, "no" },
  { Configuration::PressureAlgorithm::Berendsen, "berendsen" },
  { Configuration::PressureAlgorithm::ParrinelloRahman, "Parrinello-Rahman" },
  { Configuration::PressureAlgorithm::Bussi, "C-rescale" },
});

QVariant pressureAlgorithmFrom(const QString& value)
{
  return QVariant::fromValue(pressureAlgorithmBimap.right.at(value));
}

QString toString(Configuration::PressureAlgorithm algorithm)
{
  return pressureAlgorithmBimap.left.at(algorithm);
}

QString toString(Configuration::Type type, bool shortVersion)
{
  const static QMap<Configuration::Type, QString> map = {
    { Configuration::Type::None, "None" },
    { Configuration::Type::Minimisation, "Minimisation" },
    { Configuration::Type::NVE, "Microcanonical (NVE)" },
    { Configuration::Type::NVT, "Canonical (NVT)" },
    { Configuration::Type::NPT, "Gibbs/Isobaric-isothermal (NPT)" },
  };

  const static QMap<Configuration::Type, QString> shortMap = {
    { Configuration::Type::None, "" },
    { Configuration::Type::Minimisation, "min" },
    { Configuration::Type::NVE, "nve" },
    { Configuration::Type::NVT, "nvt" },
    { Configuration::Type::NPT, "npt" },
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
makeBimap<Configuration::PressureCouplingType, QString>({
  { Configuration::PressureCouplingType::Isotropic, "isotropic" },
  { Configuration::PressureCouplingType::SemiIsoTropic, "semiisotropic" },
  { Configuration::PressureCouplingType::Anisotropic, "anisotropic" },
  { Configuration::PressureCouplingType::SurfaceTension, "surface-tension" },
});

QVariant pressureCouplingTypeFrom(const QString& value)
{
  return QVariant::fromValue(pressureCouplingTypeBimap.right.at(value));
}

QString toString(Configuration::PressureCouplingType type)
{
  return pressureCouplingTypeBimap.left.at(type);
}

//QDataStream &operator<<(QDataStream &out, const Configuration &model)
//{
//  const Model::Serializable& tmp = model;
//  out << tmp;
//
//  const auto& groups = model.getTemperatureCouplingGroups();
//  int noOfGroups = groups.size();
//  out << noOfGroups;
//  for (int groupIndex = 0; groupIndex < noOfGroups; groupIndex++)
//  {
//    out << *(groups[groupIndex]);
//  }
//
//
//  return out;
//}
//
//QDataStream &operator>>(QDataStream &in, Configuration& model)
//{
//  Model::Serializable& tmp = model;
//  in >> tmp;
//
//  int noOfGroups;
//  in >> noOfGroups;
//
//  auto& groups = model.getTemperatureCouplingGroups();
//  groups.clear();
//  for (int groupIndex = 0; groupIndex < noOfGroups; groupIndex++)
//  {
//    auto group = model.addTemperatureCouplingGroup();
//    in >> (*group);
//  }
//
//  return in;
//}

const static auto temperatureAlgorithmBimap =
makeBimap<Configuration::TemperatureAlgorithm, QString>({
  { Configuration::TemperatureAlgorithm::None, "no" },
  { Configuration::TemperatureAlgorithm::Berendsen, "berendsen" },
  { Configuration::TemperatureAlgorithm::NoseHoover, "nose-hoover" },
  { Configuration::TemperatureAlgorithm::Andersen, "andersen" },
  { Configuration::TemperatureAlgorithm::AndersenMassive, "andersen-massive" },
  { Configuration::TemperatureAlgorithm::VelocityRescale, "V-rescale" },
});

QVariant temperatureAlgorithmFrom(const QString& value)
{
  return QVariant::fromValue(temperatureAlgorithmBimap.right.at(value));
}

QString toString(Configuration::TemperatureAlgorithm algorithm)
{
  return temperatureAlgorithmBimap.left.at(algorithm);
}

const static auto electrostaticAlgorithmBimap =
makeBimap<Configuration::ElectrostaticAlgorithm, QString>({
  { Configuration::ElectrostaticAlgorithm::CutOff, "Cut-Off" },
  { Configuration::ElectrostaticAlgorithm::Ewald, "Ewald" },
  { Configuration::ElectrostaticAlgorithm::PME, "PME" },
  { Configuration::ElectrostaticAlgorithm::P3MAD, "P3M-AD" },
  { Configuration::ElectrostaticAlgorithm::ReactionField, "Reaction-Field" },
});

QVariant electrostaticAlgorithmFrom(const QString& value)
{
  return QVariant::fromValue(electrostaticAlgorithmBimap.right.at(value));
}

QString toString(Configuration::ElectrostaticAlgorithm algorithm)
{
  return electrostaticAlgorithmBimap.left.at(algorithm);
}

const static auto vdwAlgorithmBimap =
makeBimap<Configuration::VdwAlgorithm, QString>({
  { Configuration::VdwAlgorithm::CutOff, "Cut-Off" },
  { Configuration::VdwAlgorithm::PME, "PME" },
});

QVariant vdwAlgorithmFrom(const QString& value)
{
  return QVariant::fromValue(vdwAlgorithmBimap.right.at(value));
}

QString toString(Configuration::VdwAlgorithm algorithm)
{
  return vdwAlgorithmBimap.left.at(algorithm);
}

const static auto vdwModifierBimap =
makeBimap<Configuration::VdwModifier, QString>({
  { Configuration::VdwModifier::None, "None" },
  { Configuration::VdwModifier::PotentialShift, "Potential-Shift" },
  { Configuration::VdwModifier::ForceSwitch, "Force-Switch" },
  { Configuration::VdwModifier::PotentialSwitch, "Potential-Switch" },
});

QVariant vdwModifierFrom(const QString& value)
{
  return QVariant::fromValue(vdwModifierBimap.right.at(value));
}

QString toString(Configuration::VdwModifier algorithm)
{
  return vdwModifierBimap.left.at(algorithm);
}

} }
