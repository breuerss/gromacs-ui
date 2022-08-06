#include "step.h"
#include "simulationtype.h"

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
  if ((*this)["simulationType"].value<SimulationType::Type>() == SimulationType::Minimisation)
  {
    name = "Minimisation";
  }

  return name;
}

