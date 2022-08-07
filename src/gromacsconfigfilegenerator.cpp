#include "gromacsconfigfilegenerator.h"

#include "model/simulationtype.h"

#include <QFile>
#include <QTextStream>

void GromacsConfigFileGenerator::generate(std::shared_ptr<Step> step, const QString& fileName)
{
    QFile file(fileName);
    file.open(QFile::WriteOnly);
    QTextStream writer(&file);

    SimulationType simulationType = step->property("simulationType").value<SimulationType>();
    if (simulationType != SimulationType::None)
    {
        writeLine(writer, "integrator", step->property("algorithm").toString());
        writeLine(writer, "nsteps", step->property("numberOfSteps").toString());

        // output control
        writeLine(writer, "nstenergy", step->property("energyOutputFrequency").toString());
        writeLine(writer, "nstxout", step->property("positionOutputFrequency").toString());
        writeLine(writer, "nstxout-compressed", step->property("positionOutputFrequency").toString());

        writeLine(writer, "nstvout", step->property("velocityOutputFrequency").toString());
        writeLine(writer, "nstfout", step->property("forceOutputFrequency").toString());
        // TODO make logging freq configurable
        writeLine(writer, "nstlog", QString::number(1000));

        // electrostatics and VdW
        writeLine(writer, "coulombtype", step->property("electrostaticAlgorithm").toString());
        writeLine(writer, "fourierSpacing", step->property("fourierSpacing").toString());

        writeLine(writer, "rcoulomb", step->property("electrostaticCutoffRadius").toString());
        writeLine(writer, "rvdw", step->property("vdwCutoffRadius").toString());
    }

    if (simulationType == SimulationType::Minimisation)
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
