#include "gromacsconfigfilegenerator.h"

#include "model/simulation.h"

#include <QFile>
#include <QTextStream>

void GromacsConfigFileGenerator::generate(
  std::shared_ptr<Model::Simulation> step,
  const QString& fileName
  )
{
  QFile file(fileName);
  file.open(QFile::WriteOnly);
  QTextStream writer(&file);

  using Model::Simulation;
  Simulation::Type simulationType = step->property("simulationType").value<Simulation::Type>();
  if (simulationType != Simulation::Type::None)
  {
    writeLine(writer, "integrator", toString(step->property("algorithm").value<Simulation::Algorithm>()));
    writeLine(writer, "nsteps", step->property("numberOfSteps").toString());
    writeLine(writer, "dt", QString::number(step->property("timeStep").value<double>() / 1000));

    // output control
    writeLine(writer, "nstenergy", QString::number(step->property("energyOutputFrequency").value<int>()));
    writeLine(writer, "nstxout", QString::number(step->property("positionOutputFrequency").value<int>()));
    writeLine(writer, "nstxout-compressed", QString::number(step->property("compressedPositionOutputFrequency").value<int>()));

    writeLine(writer, "nstvout", QString::number(step->property("velocityOutputFrequency").value<int>()));
    writeLine(writer, "nstfout", QString::number(step->property("forceOutputFrequency").value<int>()));
    writeLine(writer, "nstlog", QString::number(step->property("logOutputFrequency").value<int>()));

    // electrostatics and VdW
    writeLine(writer, "coulombtype", step->property("electrostaticAlgorithm").toString());
    writeLine(writer, "fourierSpacing", step->property("fourierSpacing").toString());

    writeLine(writer, "rcoulomb", step->property("electrostaticCutoffRadius").toString());
    writeLine(writer, "rvdw", step->property("vdwCutoffRadius").toString());

    using Model::Simulation;

    // pressure
    Simulation::PressureAlgorithm pressureAlgorithm =
      step->property("pressureAlgorithm").value<Simulation::PressureAlgorithm>();
    writeLine(writer, "pcoupl", toString(pressureAlgorithm));
    if (pressureAlgorithm != Simulation::PressureAlgorithm::None)
    {
      writeLine(writer, "pcoupltype", toString(step->property("pressureCouplingType").value<Simulation::PressureCouplingType>()));
      writeLine(writer, "tau-p", QString::number(step->property("pressureUpdateInterval").value<double>()));
      writeLine(writer, "ref-p", QString::number(step->property("pressure").value<double>()));
      writeLine(writer, "compressibility", "4.5e-5");
    }

    // temperature
    Simulation::TemperatureAlgorithm temperatureAlgorithm =
      step->property("temperatureAlgorithm").value<Simulation::TemperatureAlgorithm>();
    writeLine(writer, "tcoupl", toString(temperatureAlgorithm));
    if (temperatureAlgorithm != Simulation::TemperatureAlgorithm::None)
    {
      QStringList groupNames;
      QStringList temperatures;
      QStringList updateIntervals;

      const auto& groups = step->getTemperatureCouplingGroups();
      for (auto group : groups)
      {
        groupNames << toString(group->property("group").value<Model::TemperatureCouplingGroup::Group>());
        temperatures << QString::number(group->property("temperature").value<double>());
        updateIntervals << QString::number(group->property("updateInterval").value<double>());
      }

      writeLine(writer, "tc-grps", groupNames.join(" "));
      writeLine(writer, "tau-t", updateIntervals.join(" "));
      writeLine(writer, "ref-t", temperatures.join(" "));
    }
  }

  if (simulationType == Model::Simulation::Type::Minimisation)
  {
    writeLine(writer, "emtol", step->property("minimisationMaximumForce").toString());
    writeLine(writer, "emstep", step->property("minimisationStepSize").toString());
  }

  file.close();
}

void GromacsConfigFileGenerator::writeLine(QTextStream& writer, const QString& key, const QString& value)
{
  writer << qSetFieldWidth(25) << Qt::left << key << qSetFieldWidth(0) << " = " << value << ";" << Qt::endl;
}
