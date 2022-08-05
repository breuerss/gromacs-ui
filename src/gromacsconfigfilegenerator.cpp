#include "gromacsconfigfilegenerator.h"

#include "model/simulationtype.h"

#include <QFile>
#include <QTextStream>

void GromacsConfigFileGenerator::generate(std::shared_ptr<Step> step, const QString& fileName)
{
    QFile file(fileName);
    file.open(QFile::WriteOnly);
    QTextStream writer(&file);

    SimulationType::Type simulationType = (*step)["simulationType"].value<SimulationType::Type>();
    if (simulationType == SimulationType::Minimisation)
    {

        writeLine(writer, "title", "Minimisation");
        writeLine(writer, "integrator", (*step)["algorithm"].toString());
        writeLine(writer, "emtol", (*step)["minimisationMaximumForce"].toString());
        writeLine(writer, "emstep", (*step)["minimisationStepSize"].toString());
    }

    if (simulationType != SimulationType::None)
    {
        writeLine(writer, "nsteps", (*step)["numberOfSteps"].toString());

        // output control
        writeLine(writer, "nstenergy", (*step)["energyOutputFrequency"].toString());
        writeLine(writer, "nstxout", (*step)["positionOutputFrequency"].toString());
        writeLine(writer, "nstvout", (*step)["velocityOutputFrequency"].toString());
        writeLine(writer, "nstfout", (*step)["forceOutputFrequency"].toString());
        // TODO make logging freq configurable
        writeLine(writer, "nstlog", QString::number(1000));

        // electrostatics and VdW
        writeLine(writer, "coulombtype", (*step)["electrostaticAlgorithm"].toString());
        writeLine(writer, "fourierSpacing", (*step)["fourierSpacing"].toString());

        writeLine(writer, "rcoulomb", (*step)["electrostaticCutoffRadius"].toString());
        writeLine(writer, "rvdw", (*step)["vdwCutoffRadius"].toString());
    }

    file.close();
}

void GromacsConfigFileGenerator::writeLine(QTextStream& writer, const QString& key, const QString& value)
{
    writer << qSetFieldWidth(25) << Qt::left << key << qSetFieldWidth(0) << " = " << value << ";" << Qt::endl;
}
