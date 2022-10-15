#include "gromacsconfigfilegenerator.h"

#include "appprovider.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QProcess>
#include <QTemporaryFile>
#include <memory>
#include <stdexcept>

const QMap<QString, QString> GromacsConfigFileGenerator::optionsMap = {
  { "integrator", "algorithm" },
  { "nsteps", "numberOfSteps" },
  { "dt", "timeStep" },
  { "nstenergy", "energyOutputFrequency" },
  { "nstxout", "positionOutputFrequency" },
  { "nstxout-compressed", "compressedPositionOutputFrequency" },
  { "nstvout", "velocityOutputFrequency" },
  { "nstfout", "forceOutputFrequency" },
  { "nstlog", "logOutputFrequency" },
  { "coulombtype", "electrostaticAlgorithm" },
  { "fourierSpacing", "fourierSpacing" },
  { "rcoulomb", "electrostaticCutoffRadius" },
  { "ewald-rtol", "electrostaticEwaldRtol" },
  { "vdwtype", "vdwAlgorithm" },
  { "vdw-modifier", "vdwModifier" },
  { "vdw-switch", "vdwSwitchRadius" },
  { "rvdw", "vdwCutoffRadius" },
  { "ewald-rtol-lj", "vdwEwaldRtol" },
  { "pcoupl", "pressureAlgorithm" },
  { "pcoupltype", "pressureCouplingType" },
  { "tau-p", "pressureUpdateInterval" },
  { "ref-p", "pressure" },
  { "compressibility", "compressibility" },
  { "tcoupl", "temperatureAlgorithm" },
  { "tc-grps", "group" },
  { "tau-t", "updateInterval" },
  { "ref-t", "temperature" },
  { "emtol", "minimisationMaximumForce" },
  { "emstep", "minimisationStepSize" },
  { "pme-order", "pmeOrder" },

  { "constraints", "constraints" },
  { "constraint-algorithm", "constraintAlgorithm" },
  { "lincs-order", "lincsOrder" },
  { "lincs-iter", "lincsIter" },
  { "lincs-warnangle", "lincsWarnAngle" },
  { "shake-tol", "shakeTolerance" },
};

GromacsConfigFileGenerator::GromacsConfigFileGenerator(
  Pipeline::Simulation::Configuration* newConfiguration
  )
  : configuration(newConfiguration)
{
}

const QMap<QString, std::function<QVariant(const QString&)>>
GromacsConfigFileGenerator::conversionMap = {
  { "integrator", Pipeline::Simulation::simulationAlgorithmFrom },
  {
    "dt", [] (const QString& timeStep) {
      return QVariant::fromValue(timeStep.toFloat() * 1000);
    },
  },
  { "pcoupl", Pipeline::Simulation::pressureAlgorithmFrom },
  { "pcoupltype", Pipeline::Simulation::pressureCouplingTypeFrom },
  { "tcoupl", Pipeline::Simulation::temperatureAlgorithmFrom },
  { "tc-grps", Pipeline::Simulation::temperatureGroupFrom },
  { "coulombtype", Pipeline::Simulation::electrostaticAlgorithmFrom},
  { "vdwtype", Pipeline::Simulation::vdwAlgorithmFrom},
  { "vdw-modifier", Pipeline::Simulation::vdwModifierFrom},

  { "constraints", Pipeline::Simulation::constraintTargetFrom},
  { "constraint-algorithm", Pipeline::Simulation::constraintAlgorithmFrom},
};

const QList<QString> GromacsConfigFileGenerator::temperatureCouplingOptions = {
  "tc-grps",
  "tau-t",
  "ref-t",
};

void GromacsConfigFileGenerator::generate(
  const QString& fileName
  )
{
  QFile file(fileName);
  file.open(QFile::WriteOnly);
  QTextStream writer(&file);

  using Simulation = Pipeline::Simulation::Configuration;
  Simulation::Type simulationType = configuration->property("simulationType").value<Simulation::Type>();
  if (simulationType != Simulation::Type::None)
  {
    writeLine<Simulation::Algorithm>(writer, "integrator");
    writeLine(writer, "nsteps", QString::number(configuration->property("numberOfSteps").value<double>(), 'f', 0));
    writeLine(writer, "dt", QString::number(configuration->property("timeStep").value<double>() / 1000));

    // output control
    writeLine<unsigned int>(writer, "nstenergy");
    writeLine<unsigned int>(writer, "nstxout");
    writeLine<unsigned int>(writer, "nstxout-compressed");

    writeLine<unsigned int>(writer, "nstvout");
    writeLine<unsigned int>(writer, "nstfout");
    writeLine<unsigned int>(writer, "nstlog");

    // electrostatics
    writeLine<Simulation::ElectrostaticAlgorithm>(writer, "coulombtype");
    writeLine<float>(writer, "rcoulomb");
    writeLine<double>(writer, "ewald-rtol");

    // PME
    writeLine<float>(writer, "fourierSpacing");
    writeLine<unsigned int>(writer, "pme-order");

    //  and VdW
    writeLine<Simulation::VdwAlgorithm>(writer, "vdwtype");
    writeLine<Simulation::VdwModifier>(writer, "vdw-modifier");
    writeLine<float>(writer, "rvdw");
    writeLine<double>(writer, "ewald-rtol-lj");

    // pressure
    writeLine<Simulation::PressureAlgorithm>(writer, "pcoupl");
    auto pressureAlgorithm = configuration->property("pressureAlgorithm")
      .value<Simulation::PressureAlgorithm>();
    if (pressureAlgorithm != Simulation::PressureAlgorithm::None)
    {
      writeLine<Simulation::PressureCouplingType>(writer, "pcoupltype");
      writeLine<float>(writer, "tau-p");
      writeLine<float>(writer, "ref-p");
      writeLine<double>(writer, "compressibility");
    }

    using ConstraintTarget = Simulation::ConstraintTarget;
    const auto constraints = configuration->property("constraints")
      .value<ConstraintTarget>();
    const bool constraintActive = constraints != ConstraintTarget::None;
    if (constraintActive)
    {
      writeLine<ConstraintTarget>(writer, "constraints");
      using ConstraintAlgorithm = Simulation::ConstraintAlgorithm;
      writeLine<ConstraintAlgorithm>(writer, "constraint-algorithm");
      const auto algorithm = configuration
        ->property("constraintAlgorithm").value<ConstraintAlgorithm>();
      if (algorithm == ConstraintAlgorithm::LINCS)
      {
        writeLine<int>(writer, "lincs-order");
        writeLine<int>(writer, "lincs-iter");
        writeLine<int>(writer, "lincs-warnangle");
      }
      if (algorithm == ConstraintAlgorithm::SHAKE)
      {
        writeLine<double>(writer, "shake-tol");
      }
      writeLine(
        writer, "morse",
        configuration->property("morsePotential").toBool() ? "yes" : "no");
      writeLine(
        writer, "continuation",
        configuration->property("continuation").toBool() ? "yes" : "no");
    }

    // temperature
    Simulation::TemperatureAlgorithm temperatureAlgorithm =
      configuration->property("temperatureAlgorithm").value<Simulation::TemperatureAlgorithm>();
    writeLine<Simulation::TemperatureAlgorithm>(writer, "tcoupl");
    if (temperatureAlgorithm != Simulation::TemperatureAlgorithm::None)
    {
      QStringList groupNames;
      QStringList temperatures;
      QStringList updateIntervals;

      const auto& groups = configuration->getTemperatureCouplingGroups();
      for (auto group : groups)
      {
        groupNames << toString(group->property("group").value<Pipeline::Simulation::TemperatureCouplingGroup::Group>());
        temperatures << QString::number(group->property("temperature").value<double>());
        updateIntervals << QString::number(group->property("updateInterval").value<double>());
      }

      writeLine(writer, "tc-grps", groupNames.join(" "));
      writeLine(writer, "tau-t", updateIntervals.join(" "));
      writeLine(writer, "ref-t", temperatures.join(" "));
    }
  }

  if (simulationType == Simulation::Type::Minimisation)
  {
    writeLine<float>(writer, "emtol");
    writeLine<float>(writer, "emstep");
  }

  file.close();
}

std::shared_ptr<Pipeline::Simulation::Configuration>
GromacsConfigFileGenerator::createFrom(const QString& fileName)
{
  auto configuration = std::make_shared<Pipeline::Simulation::Configuration>();
  GromacsConfigFileGenerator(configuration.get()).setFromMdpFile(fileName);
  return configuration;
}

void GromacsConfigFileGenerator::setFromMdpFile(
  const QString& fileName
  )
{
  QFile file(fileName);
  if (file.exists())
  {
    file.open(QFile::ReadOnly);
    QTextStream stream(&file);
    while(!stream.atEnd())
    {
      QString line = stream.readLine().trimmed();
      if (line.isEmpty())
      {
        continue;
      }
      if (line.startsWith(";"))
      {
        continue;
      }

      QStringList keyValue = line.split("=");
      QString option = keyValue[0].trimmed();
      if (!optionsMap.contains(option))
      {
        qDebug () << option << "not supported";
        continue;
      }

      QString inputValueString = keyValue[1].trimmed().remove(";").trimmed();
      if (temperatureCouplingOptions.contains(option))
      {
        QStringList groupsInputValues = inputValueString
          .split(QRegExp("\\s+"), Qt::SkipEmptyParts);
        auto& temperatureCouplingGroups = configuration->getTemperatureCouplingGroups();
        for (int groupIndex = temperatureCouplingGroups.size();
             groupIndex < groupsInputValues.size(); groupIndex++)
        {
          configuration->addTemperatureCouplingGroup();
        }

        for (int valueIndex = 0; valueIndex < groupsInputValues.size(); valueIndex++)
        {
          QVariant inputValue = createValueFrom(option, groupsInputValues[valueIndex]);
          if (inputValue.isNull())
          {
            continue;
          }

          temperatureCouplingGroups[valueIndex]
            ->setProperty(optionsMap.value(option).toStdString().c_str(), inputValue);
        }


        continue;
      }

      QVariant inputValue = createValueFrom(option, inputValueString);
      if (inputValue.isNull())
      {
        continue;
      }

      configuration->setProperty(optionsMap.value(option).toStdString().c_str(), inputValue);
    }
    file.close();

    using Simulation = Pipeline::Simulation::Configuration;
    auto simulationType = Simulation::Type::None;
    if (configuration->isMinimisation())
    {
      simulationType = Simulation::Type::Minimisation;
    }
    else if (
      configuration->property("temperatureAlgorithm").value<Simulation::TemperatureAlgorithm>() !=
      Simulation::TemperatureAlgorithm::None)
    {
      simulationType = Simulation::Type::NVT;
      if (
        configuration->property("pressureAlgorithm").value<Simulation::PressureAlgorithm>() !=
        Simulation::PressureAlgorithm::None
        )
      {
        simulationType = Simulation::Type::NPT;
      }
    }
    configuration->setProperty("type", QVariant::fromValue(simulationType));
  }
  else
  {
    qDebug() << fileName << "does not exist.";
  }


}

QVariant GromacsConfigFileGenerator::createValueFrom(
  const QString& option,
  const QString& inputValueString)
{
  QVariant inputValue = QVariant::fromValue(inputValueString);
  if (conversionMap.contains(option))
  {
    try
    {
      inputValue = conversionMap[option](inputValue.toString());
    }
    catch (const std::out_of_range& exc)
    {
      qDebug() << "Value" << inputValue.toString() << "for option" << option << "unknown.";
      inputValue = QVariant();
    }
  }
  return inputValue;
}

void GromacsConfigFileGenerator::setFromTprFile(
  const QString& fileName
  )
{
  QProcess convert;
  QString command = AppProvider::get("gmx");
  if (command.isEmpty())
  {
    qDebug() << "Cannot import from tpr file. gmx command not found.";
    return;
  }

  QTemporaryFile tmpfile;
  tmpfile.open();
  QString temporaryFileName = tmpfile.fileName();

  QStringList args;
  args << "-s" << fileName;
  args << "-om" << temporaryFileName;
  convert.start(command, args);
  convert.waitForFinished();
  if (convert.exitCode() != 0)
  {
    qDebug() << "Cannot import from tpr file. Execution of " << command << "failed.";
    return;
  }
  setFromMdpFile(temporaryFileName);
}

void GromacsConfigFileGenerator::writeLine(QTextStream& writer, const QString& key, const QString& value)
{
  writer << qSetFieldWidth(25) << Qt::left << key << qSetFieldWidth(0) << " = " << value << ";" << Qt::endl;
}

