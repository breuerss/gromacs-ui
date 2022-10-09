#include "configuration.h"
#include "../../../ui/simulationsetupform.h"
#include <QDebug>
#include <QMetaProperty>
#include <QJsonArray>
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

    qRegisterMetaType<ConstraintTarget>("Configuration::ConstraintTarget");
    qRegisterMetaTypeStreamOperators<int>("Configuration::ConstraintTarget");

    qRegisterMetaType<ConstraintAlgorithm>("Configuration::ConstraintAlgorithm");
    qRegisterMetaTypeStreamOperators<int>("Configuration::ConstraintAlgorithm");
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
  return Simulation::toString(simulationType);
}

QString Configuration::toString()
{
  QStringList elements;
  elements << createParagraph({tr("Type") + ": " + getName()});

  elements << createParagraph({
    tr("General"),
    keyValue(tr("Algorithm"), algorithm),
    keyValue(tr("Steps"), numberOfSteps),
  });

  if (isMinimisation())
  {
    elements << createParagraph({
      tr("Minimisation Settings"),
      keyValue(tr("Maximum Force"), minimisationMaximumForce) + " kJ/mol/nm",
      keyValue(tr("Step Size"), minimisationStepSize) + " nm",
    });
  }

  if (temperatureAlgorithm != TemperatureAlgorithm::None)
  {
    QStringList temp({
      tr("Temperature Settings"),
      keyValue(tr("Algorithm"), temperatureAlgorithm),
    });
    for (auto group : temperatureCouplingGroups)
    {
      temp << keyValue("Group", group->property("group").toString());
      temp << keyValue("Temperature", group->property("temperature").toDouble()) + " K";
      temp << keyValue("Update Interval", group->property("updateInterval").toDouble()) + " ps";
    }

    elements << createParagraph(temp);
  }

  if (pressureAlgorithm != PressureAlgorithm::None)
  {
    elements << createParagraph({
      tr("Pressure Settings"),
      keyValue(tr("Pressure"), pressure) + " bar",
      keyValue(tr("Update Interval"), pressureUpdateInterval) + " ps",
      keyValue(tr("Algorithm"), pressureAlgorithm),
      keyValue(tr("Coupling Type"), pressureCouplingType) + "</p>",
    });
  }

  elements << createParagraph({
    tr("Electrostatic Potential"),
    keyValue(tr("Electrostatic Algorithm"), electrostaticAlgorithm),
    keyValue(tr("Cutoff Radius"), electrostaticCutoffRadius) + " nm",
    keyValue(tr("Ewald Tolerance"), electrostaticEwaldRtol),
  });

  elements << createParagraph({
    tr("Van der Waals Interaction"),
    keyValue(tr("Algorithm"), vdwAlgorithm),
    keyValue(tr("Modifier"), vdwModifier),
    keyValue(tr("Cutoff Radius"), vdwCutoffRadius) + " nm",
    keyValue(tr("Switch Radius"), vdwSwitchRadius) + " nm",
    keyValue(tr("Ewald Tolerance"), vdwEwaldRtol),
  });

  elements << createParagraph({
    tr("Particle Mesh Ewald Settings"),
    keyValue(tr("Fourier Spacing"), fourierSpacing) + " nm",
    keyValue(tr("Order"), pmeOrder),
  });

  QStringList constraintList({
    tr("Constraints"),
    keyValue(tr("Constraint Target"), constraints),
  });

  if (constraints != ConstraintTarget::None)
  {
    constraintList <<
      keyValue(tr("Algorithm"), constraintAlgorithm);
    switch(constraintAlgorithm)
    {
      case ConstraintAlgorithm::LINCS:
        {
          constraintList << keyValue(tr("LINCS Order"), lincsOrder)
            << keyValue(tr("LINCS Iteration"), lincsIter)
            << keyValue(tr("LINCS Warn Angle"), lincsWarnAngle);
          break;
        }
      case ConstraintAlgorithm::SHAKE:
        {
          constraintList << keyValue(tr("SHAKE Tolerance"), shakeTolerance);
          break;
        }
      case ConstraintAlgorithm::None:
      default:
        break;
    }
    constraintList << keyValue(tr("Continuation"), continuation ? tr("yes") : tr("no"))
      << keyValue(tr("Convert to Morse Potential"), morsePotential ? tr("yes") : tr("no"));
  }
  elements << createParagraph(constraintList);

  return elements.join("");
}

QString Configuration::createParagraph(const QStringList& elements)
{
  return "<p style='margin-top: 0px'>"
    + elements.join("<br>")
    + "</p>";
}

QString Configuration::getTypeAsString() const
{
  return Simulation::toString(simulationType, true);
}

QWidget* Configuration::getUI()
{
  return new SimulationSetupForm(this);
}

bool Configuration::isMinimisation() const
{
  return algorithm == Configuration::Algorithm::SteepestDescent ||
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

using Algorithm = Configuration::Algorithm;
const static auto simulationAlgorithmBimap = makeBimap<Algorithm, QString>({
  { Algorithm::None, "" },
  { Algorithm::SteepestDescent, "steep" },
  { Algorithm::ConjugateGradient, "cg" },
  { Algorithm::LeapFrog, "md" },
  { Algorithm::StochasticDynamics, "sd" },
});

QVariant simulationAlgorithmFrom(const QString& value)
{
  return QVariant::fromValue(simulationAlgorithmBimap.right.at(value));
}

QString toString(Configuration::Algorithm algorithm)
{
  return simulationAlgorithmBimap.left.at(algorithm);
}

const static auto simulationAlgorithmLabels = QMap<Algorithm, QString>({
  { Algorithm::None, "None" },
  { Algorithm::SteepestDescent, "Steepest Descent" },
  { Algorithm::ConjugateGradient, "Conjugate Gradient" },
  { Algorithm::LeapFrog, "Leap Frog" },
  { Algorithm::StochasticDynamics, "Stochastic Dynamics" },
});

QString toLabel(Configuration::Algorithm algorithm)
{
  return simulationAlgorithmLabels[algorithm];
}


using PressureAlgorithm = Configuration::PressureAlgorithm;
const static auto pressureAlgorithmBimap = makeBimap<PressureAlgorithm, QString>({
  { PressureAlgorithm::None, "no" },
  { PressureAlgorithm::Berendsen, "berendsen" },
  { PressureAlgorithm::ParrinelloRahman, "Parrinello-Rahman" },
  { PressureAlgorithm::Bussi, "C-rescale" },
});

QVariant pressureAlgorithmFrom(const QString& value)
{
  return QVariant::fromValue(pressureAlgorithmBimap.right.at(value));
}

QString toString(Configuration::PressureAlgorithm algorithm)
{
  return pressureAlgorithmBimap.left.at(algorithm);
}

const static auto pressureAlgorithmLabels = QMap<Configuration::PressureAlgorithm, QString>({
  { PressureAlgorithm::None, "None" },
  { PressureAlgorithm::Berendsen, "berendsen" },
  { PressureAlgorithm::ParrinelloRahman, "Parrinello-Rahman" },
  { PressureAlgorithm::Bussi, "C-rescale" },
});

QString toLabel(Configuration::PressureAlgorithm algorithm)
{
  return pressureAlgorithmLabels[algorithm];
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

using PressureCouplingType = Configuration::PressureCouplingType;
const static auto pressureCouplingTypeBimap =
makeBimap<PressureCouplingType, QString>({
  { PressureCouplingType::Isotropic, "isotropic" },
  { PressureCouplingType::SemiIsoTropic, "semiisotropic" },
  { PressureCouplingType::Anisotropic, "anisotropic" },
  { PressureCouplingType::SurfaceTension, "surface-tension" },
});

QVariant pressureCouplingTypeFrom(const QString& value)
{
  return QVariant::fromValue(pressureCouplingTypeBimap.right.at(value));
}

QString toString(Configuration::PressureCouplingType type)
{
  return pressureCouplingTypeBimap.left.at(type);
}

const static auto pressureCouplingTypeLabels =
QMap<PressureCouplingType, QString>({
  { PressureCouplingType::Isotropic, "Isotropic" },
  { PressureCouplingType::SemiIsoTropic, "Semiisotropic" },
  { PressureCouplingType::Anisotropic, "Anisotropic" },
  { PressureCouplingType::SurfaceTension, "Surface-Tension" },
});

QString toLabel(Configuration::PressureCouplingType type)
{
  return pressureCouplingTypeLabels[type];
}

void Configuration::toObject(QJsonObject& out)
{
  out << Configuration::shared_from_this();
}

void Configuration::fromObject(QJsonObject& in)
{
  in >> Configuration::shared_from_this();
}

QJsonObject &operator<<(QJsonObject &out, const std::shared_ptr<Configuration>& model)
{
  const auto& tmp = std::dynamic_pointer_cast<Model::Serializable>(model);
  out << tmp;

  const auto& groups = model->getTemperatureCouplingGroups();
  QJsonArray temperatureGroups;
  for (const auto& group: groups)
  {
    QJsonObject groupObject;
    groupObject << group;
    temperatureGroups.append(groupObject);
  }
  out["temperatureGroups"] = temperatureGroups;

  return out;
}

QJsonObject &operator>>(QJsonObject &in, std::shared_ptr<Configuration> model)
{
  auto tmp = std::dynamic_pointer_cast<Model::Serializable>(model);
  in >> tmp;

  auto& groups = model->getTemperatureCouplingGroups();
  groups.clear();
  if (in.contains("temperatureGroups") && in["temperatureGroups"].isArray())
  {
    QJsonArray arr = in["temperatureGroups"].toArray();
    for (const auto& entry: arr)
    {
      std::shared_ptr<Model::Serializable> group = model->addTemperatureCouplingGroup();
      auto obj = entry.toObject();
      obj >> group;
    }
  }

  return in;
}

using TemperatureAlgorithm = Configuration::TemperatureAlgorithm;
const static auto temperatureAlgorithmBimap =
makeBimap<TemperatureAlgorithm, QString>({
  { TemperatureAlgorithm::None, "no" },
  { TemperatureAlgorithm::Berendsen, "berendsen" },
  { TemperatureAlgorithm::NoseHoover, "nose-hoover" },
  { TemperatureAlgorithm::Andersen, "andersen" },
  { TemperatureAlgorithm::AndersenMassive, "andersen-massive" },
  { TemperatureAlgorithm::VelocityRescale, "V-rescale" },
});

QVariant temperatureAlgorithmFrom(const QString& value)
{
  return QVariant::fromValue(temperatureAlgorithmBimap.right.at(value));
}

QString toString(Configuration::TemperatureAlgorithm algorithm)
{
  return temperatureAlgorithmBimap.left.at(algorithm);
}

const static auto temperatureAlgorithmLabels =
QMap<TemperatureAlgorithm, QString>({
  { TemperatureAlgorithm::None, "None" },
  { TemperatureAlgorithm::Berendsen, "Berendsen" },
  { TemperatureAlgorithm::NoseHoover, "Nose-Hoover" },
  { TemperatureAlgorithm::Andersen, "Andersen" },
  { TemperatureAlgorithm::AndersenMassive, "Andersen-Massive" },
  { TemperatureAlgorithm::VelocityRescale, "V-rescale" },
});

QString toLabel(Configuration::TemperatureAlgorithm algorithm)
{
  return temperatureAlgorithmLabels[algorithm];
}

using ElectrostaticAlgorithm = Configuration::ElectrostaticAlgorithm;
const static auto electrostaticAlgorithmBimap =
makeBimap<ElectrostaticAlgorithm, QString>({
  { ElectrostaticAlgorithm::CutOff, "Cut-Off" },
  { ElectrostaticAlgorithm::Ewald, "Ewald" },
  { ElectrostaticAlgorithm::PME, "PME" },
  { ElectrostaticAlgorithm::P3MAD, "P3M-AD" },
  { ElectrostaticAlgorithm::ReactionField, "Reaction-Field" },
});

QString toLabel(Configuration::ElectrostaticAlgorithm algorithm)
{
  return toString(algorithm);
}

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

QString toLabel(Configuration::VdwAlgorithm algorithm)
{
  return toString(algorithm);
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

QString toLabel(Configuration::VdwModifier algorithm)
{
  return toString(algorithm);
}

using ConstraintTarget = Configuration::ConstraintTarget;
const static auto constraintTargetBimap =
makeBimap<ConstraintTarget, QString>({
  { ConstraintTarget::None, "none" },
  { ConstraintTarget::HydrogenBonds, "h-bonds" },
  { ConstraintTarget::AllBonds, "all-bonds" },
  { ConstraintTarget::HydrogenAngles, "h-angles" },
  { ConstraintTarget::AllAngles, "all-angles" },
});

QVariant constraintTargetFrom(const QString& value)
{
  return QVariant::fromValue(constraintTargetBimap.right.at(value));
}

QString toString(Configuration::ConstraintTarget target)
{
  return constraintTargetBimap.left.at(target);
}

const static auto constraintTargetLabel =
QMap<ConstraintTarget, QString>({
  { ConstraintTarget::None, "None" },
  { ConstraintTarget::HydrogenBonds, "Hydrogen Bonds" },
  { ConstraintTarget::AllBonds, "All Bonds" },
  { ConstraintTarget::HydrogenAngles, "Hydrogen Angles" },
  { ConstraintTarget::AllAngles, "All Angles" },
});

QString toLabel(Configuration::ConstraintTarget target)
{
  return constraintTargetLabel[target];
}

using ConstraintAlgorithm = Configuration::ConstraintAlgorithm;
const static auto constraintAlgorithmBimap =
makeBimap<ConstraintAlgorithm, QString>({
  { ConstraintAlgorithm::None, "" },
  { ConstraintAlgorithm::LINCS, "LINCS" },
  { ConstraintAlgorithm::SHAKE, "SHAKE" },
});

QVariant constraintAlgorithmFrom(const QString& value)
{
  return QVariant::fromValue(constraintAlgorithmBimap.right.at(value));
}

QString toString(Configuration::ConstraintAlgorithm algorithm)
{
  return constraintAlgorithmBimap.left.at(algorithm);
}

QString toLabel(Configuration::ConstraintAlgorithm algorithm)
{
  return toString(algorithm);
}

} }
