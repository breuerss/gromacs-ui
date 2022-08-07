#include "gromacsconfigfilegenerator.h"

#include "model/simulationtype.h"
#include "model/step.h"

#include <QFile>
#include <QTextStream>

void GromacsConfigFileGenerator::generate(
    std::shared_ptr<Model::Step> step,
    const QString& fileName
    )
{
    QFile file(fileName);
    file.open(QFile::WriteOnly);
    QTextStream writer(&file);

    Model::SimulationType simulationType = step->property("simulationType").value<Model::SimulationType>();
    if (simulationType != Model::SimulationType::None)
    {
        writeLine(writer, "integrator", step->property("algorithm").toString());
        writeLine(writer, "nsteps", step->property("numberOfSteps").toString());

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
    }

    if (simulationType == Model::SimulationType::Minimisation)
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
