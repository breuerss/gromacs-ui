#include "simulation.h"
#include <QDebug>
#include <QMetaProperty>
#include <cstring>

namespace Model {

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
    registered = true;
  }
}

QString Simulation::getName() const
{
  return toString(simulationType);
}

QString Simulation::getDirectory() const
{
  return toString(simulationType, true);
}

std::vector<std::shared_ptr<TemperatureCouplingGroup>>& Simulation::getTemperatureCouplingGroups()
{
  return temperatureCouplingGroups;
}

const std::vector<std::shared_ptr<TemperatureCouplingGroup>>& Simulation::getTemperatureCouplingGroups() const
{
  return temperatureCouplingGroups;
}

QString toString(Simulation::Algorithm algorithm)
{
  const static QMap<Simulation::Algorithm, QString> map = {
    { Simulation::Algorithm::None, "" },
    { Simulation::Algorithm::SteepestDecent, "steep" },
    { Simulation::Algorithm::ConjugateGradient, "cg" },
    { Simulation::Algorithm::LeapFrog, "md" },
    { Simulation::Algorithm::StochasticDynamics, "sd" },
  };

  return map[algorithm];
}

QString toString(Simulation::PressureAlgorithm algorithm)
{
  const static QMap<Simulation::PressureAlgorithm, QString> map = {
    { Simulation::PressureAlgorithm::None, "no" },
    { Simulation::PressureAlgorithm::Berendsen, "berendsen" },
    { Simulation::PressureAlgorithm::ParrinelloRahman, "Parrinello-Rahman" },
  };

  return map[algorithm];
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

QString toString(Simulation::PressureCouplingType type)
{
  const static QMap<Simulation::PressureCouplingType, QString> map = {
    { Simulation::PressureCouplingType::Isotropic, "isotropic" },
    { Simulation::PressureCouplingType::SemiIsoTropic, "semiisotropic" },
    { Simulation::PressureCouplingType::Anisotropic, "anisotropic" },
    { Simulation::PressureCouplingType::SurfaceTension, "surface-tension" },
  };

  return map[type];
}
}
