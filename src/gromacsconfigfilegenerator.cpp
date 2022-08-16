#include "gromacsconfigfilegenerator.h"

#include "model/simulation.h"
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
  { "rvdw", "vdwCutoffRadius" },
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
};

GromacsConfigFileGenerator::GromacsConfigFileGenerator(
  std::shared_ptr<Model::Simulation> model
  )
  : model(model)
{
}

const QMap<QString, std::function<QVariant(const QString&)>>
GromacsConfigFileGenerator::conversionMap = {
  { "integrator", Model::simulationAlgorithmFrom },
  {
    "dt", [] (const QString& timeStep) {
      return QVariant::fromValue(timeStep.toFloat() * 1000);
    },
  },
  { "pcoupl", Model::pressureAlgorithmFrom },
  { "pcoupltype", Model::pressureCouplingTypeFrom },
  { "tcoupl", Model::temperatureAlgorithmFrom },
  { "tc-grps", Model::temperatureGroupFrom },
  { "coulombtype", Model::electrostaticAlgorithmFrom},
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

  using Model::Simulation;
  Simulation::Type simulationType = model->property("simulationType").value<Simulation::Type>();
  if (simulationType != Simulation::Type::None)
  {
    writeLine<Simulation::Algorithm>(writer, "integrator");
    writeLine(writer, "nsteps", QString::number(model->property("numberOfSteps").value<double>(), 'f', 0));
    writeLine(writer, "dt", QString::number(model->property("timeStep").value<double>() / 1000));

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

    // PME
    writeLine<float>(writer, "fourierSpacing");

    //  and VdW
    writeLine<float>(writer, "rvdw");

    using Model::Simulation;

    // pressure
    writeLine<Simulation::PressureAlgorithm>(writer, "pcoupl");
    Simulation::PressureAlgorithm pressureAlgorithm =
      model->property("pressureAlgorithm").value<Simulation::PressureAlgorithm>();
    if (pressureAlgorithm != Simulation::PressureAlgorithm::None)
    {
      writeLine<Simulation::PressureCouplingType>(writer, "pcoupltype");
      writeLine<float>(writer, "tau-p");
      writeLine<float>(writer, "ref-p");
      writeLine<double>(writer, "compressibility");
    }

    // temperature
    Simulation::TemperatureAlgorithm temperatureAlgorithm =
      model->property("temperatureAlgorithm").value<Simulation::TemperatureAlgorithm>();
    writeLine<Simulation::TemperatureAlgorithm>(writer, "tcoupl");
    if (temperatureAlgorithm != Simulation::TemperatureAlgorithm::None)
    {
      QStringList groupNames;
      QStringList temperatures;
      QStringList updateIntervals;

      const auto& groups = model->getTemperatureCouplingGroups();
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
    writeLine<float>(writer, "emtol");
    writeLine<float>(writer, "emstep");
  }

  file.close();
}

std::shared_ptr<Model::Simulation>
GromacsConfigFileGenerator::createFrom(const QString& fileName)
{
  auto model = std::make_shared<Model::Simulation>();
  GromacsConfigFileGenerator(model).setFromMdpFile(fileName);
  return model;
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
        auto& temperatureCouplingGroups = model->getTemperatureCouplingGroups();
        for (int groupIndex = temperatureCouplingGroups.size();
             groupIndex < groupsInputValues.size(); groupIndex++)
        {
          model->addTemperatureCouplingGroup();
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

      model->setProperty(optionsMap.value(option).toStdString().c_str(), inputValue);
    }
    file.close();

    using Model::Simulation;
    auto simulationType = Simulation::Type::None;
    if (model->isMinimisation())
    {
      simulationType = Simulation::Type::Minimisation;
    }
    else if (
      model->property("temperatureAlgorithm").value<Simulation::TemperatureAlgorithm>() !=
      Simulation::TemperatureAlgorithm::None)
    {
      simulationType = Simulation::Type::NVT;
      if (
        model->property("pressureAlgorithm").value<Simulation::PressureAlgorithm>() !=
        Simulation::PressureAlgorithm::None
        )
      {
        simulationType = Simulation::Type::NPT;
      }
    }
    model->setProperty("type", QVariant::fromValue(simulationType));
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

