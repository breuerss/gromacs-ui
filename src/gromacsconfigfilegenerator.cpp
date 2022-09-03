#include "gromacsconfigfilegenerator.h"

#include "config/simulation.h"
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
};

GromacsConfigFileGenerator::GromacsConfigFileGenerator(
  Config::Simulation* newConfiguration
  )
  : configuration(newConfiguration)
{
}

const QMap<QString, std::function<QVariant(const QString&)>>
GromacsConfigFileGenerator::conversionMap = {
  { "integrator", Config::simulationAlgorithmFrom },
  {
    "dt", [] (const QString& timeStep) {
      return QVariant::fromValue(timeStep.toFloat() * 1000);
    },
  },
  { "pcoupl", Config::pressureAlgorithmFrom },
  { "pcoupltype", Config::pressureCouplingTypeFrom },
  { "tcoupl", Config::temperatureAlgorithmFrom },
  { "tc-grps", Config::temperatureGroupFrom },
  { "coulombtype", Config::electrostaticAlgorithmFrom},
  { "vdwtype", Config::vdwAlgorithmFrom},
  { "vdw-modifier", Config::vdwModifierFrom},
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

  using Config::Simulation;
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
    writeLine<unsigned int>(writer, "pmd-order");

    //  and VdW
    writeLine<Simulation::VdwAlgorithm>(writer, "vdwtype");
    writeLine<Simulation::VdwModifier>(writer, "vdw-modifier");
    writeLine<float>(writer, "rvdw");
    writeLine<double>(writer, "ewald-rtol-lj");

    using Config::Simulation;

    // pressure
    writeLine<Simulation::PressureAlgorithm>(writer, "pcoupl");
    Simulation::PressureAlgorithm pressureAlgorithm =
      configuration->property("pressureAlgorithm").value<Simulation::PressureAlgorithm>();
    if (pressureAlgorithm != Simulation::PressureAlgorithm::None)
    {
      writeLine<Simulation::PressureCouplingType>(writer, "pcoupltype");
      writeLine<float>(writer, "tau-p");
      writeLine<float>(writer, "ref-p");
      writeLine<double>(writer, "compressibility");
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
        groupNames << toString(group->property("group").value<Config::TemperatureCouplingGroup::Group>());
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

std::shared_ptr<Config::Simulation>
GromacsConfigFileGenerator::createFrom(const QString& fileName)
{
  auto configuration = std::make_shared<Config::Simulation>();
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

    using Config::Simulation;
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
  command += "-s " + fileName;
  command += "-om " + temporaryFileName;
  convert.start(command);
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

