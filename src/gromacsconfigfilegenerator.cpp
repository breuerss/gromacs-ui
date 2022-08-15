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
};

const QList<QString> GromacsConfigFileGenerator::temperatureCouplingOptions = {
  "tc-grps",
  "tau-t",
  "ref-t",
};

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
    writeLine(writer, "nsteps", QString::number(step->property("numberOfSteps").value<double>(), 'f', 0));
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
      writeLine(writer, "compressibility", QString::number(step->property("compressibility")
                                                           .value<double>()));
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

std::shared_ptr<Model::Simulation>
GromacsConfigFileGenerator::createFrom(const QString& fileName)
{
  auto model = std::make_shared<Model::Simulation>();
  setFromMdpFile(model, fileName);
  return model;
}

void GromacsConfigFileGenerator::setFromMdpFile(
  std::shared_ptr<Model::Simulation> model,
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
  std::shared_ptr<Model::Simulation> model,
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
  setFromMdpFile(model, temporaryFileName);
}


void GromacsConfigFileGenerator::writeLine(QTextStream& writer, const QString& key, const QString& value)
{
  writer << qSetFieldWidth(25) << Qt::left << key << qSetFieldWidth(0) << " = " << value << ";" << Qt::endl;
}

