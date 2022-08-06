#include "step.h"
#include "simulationtype.h"
#include <QDebug>

Step::Step()
{
    (*this)["simulationType"] = SimulationType::None;
    (*this)["algorithm"] = "";

    (*this)["numberOfSteps"] = -1;
    (*this)["minimisationMaximumForce"] = 1000.0;
    (*this)["minimisationStepSize"] = 0.01;

    (*this)["energyOutputFrequency"] = 500;
    (*this)["positionOutputFrequency"] = 1000;
    (*this)["velocityOutputFrequency"] = 0;
    (*this)["forceOutputFrequency"] = 0;

    (*this)["electrostaticAlgorithm"] = "PME";
    (*this)["electrostaticCutoffRadius"] = 1.0;
    (*this)["fourierSpacing"] = 0.125;
    (*this)["vdwCutoffRadius"] = 1.0;
}

QString Step::getName() const
{
  // TODO update on simulationType change
  // TODO translation
  QString name("None");
  const SimulationType::Type type = (*this)["simulationType"].value<SimulationType::Type>();
  if (type == SimulationType::Minimisation)
  {
    name = "Minimisation";
  }
  else if (type == SimulationType::NVT)
  {
      name = "NVT";
  }


  return name;
}

QString Step::getDirectory() const
{
  // TODO update on simulationType change
  // TODO translation
  QString directory("None");
  const SimulationType::Type type = (*this)["simulationType"].value<SimulationType::Type>();
  if (type == SimulationType::Minimisation)
  {
    directory = "min";
  }
  else if (type == SimulationType::NVT)
  {
      directory = "nvt";
  }


  return directory;
}

QVariant& Step::operator[](const QString& key)
{
    qDebug() << "in operator []";
    if (key == "simulationType")
    {
        emit directoryChanged();
        emit nameChanged();
    }

    return QVariantMap::operator[](key);
}

QVariant Step::operator[](const QString& key) const
{
    return QVariantMap::operator[](key);
}
