#include "gromacsconfigfilegenerator.h"

#include <QFile>
#include <QTextStream>
#include "simulationtype.h"

void GromacsConfigFileGenerator::generate(std::shared_ptr<Step> step, const QString& fileName)
{
    QFile file(fileName);
    file.open(QFile::WriteOnly);
    QTextStream writer(&file);

    SimulationType::Type simulationType = (*step)["simulationType"].value<SimulationType::Type>();
    if (simulationType == SimulationType::Minimisation)
    {

        /**
         * title       = CHARMM steepest descent enrgy minimisation

; Parameters describing what to do, when to stop and what to save
integrator  = steep  ; Algorithm (steep = steepest descent minimization)
emtol       = 1000.0 ; Stop minimization when the maximum force < 1000.0 kJ/mol/nm
emstep      = 0.01   ; Minimization step size
nstenergy   = 500    ; save energies every 1.0 ps, so we can observe if we are successful
nsteps      = -1     ; run as long as we need
; Settings that make sure we run with parameters in harmony with the selected force-field
constraints             = h-bonds   ; bonds involving H are constrained
rcoulomb                = 1.2       ; short-range electrostatic cutoff (in nm)
rvdw                    = 1.2       ; short-range van der Waals cutoff (in nm)
vdw-modifier            = Force-switch ;  specific CHARMM
rvdw_switch             = 1.0       ;
DispCorr                = no        ; account for cut-off vdW scheme -
;in case of CHARMM DispCorr = EnerPres only for monolayers
coulombtype             = PME       ; Particle Mesh Ewald for long-range electrostatics
fourierspacing          = 0.15     ; grid spacing for FFT
*/
        writeLine(writer, "title", "Minimisation");
        writeLine(writer, "integrator", (*step)["algorithm"].toString());
        writeLine(writer, "emtol", (*step)["minimisationMaximumForce"].toString());
        writeLine(writer, "emstep", (*step)["minimisationStepSize"].toString());
    }

    if (simulationType != SimulationType::None)
    {
        writeLine(writer, "nsteps", (*step)["numberOfSteps"].toString());

        writeLine(writer, "nstenergy", (*step)["energyOutputFrequency"].toString());
        writeLine(writer, "nstxout", (*step)["positionOutputFrequency"].toString());
        writeLine(writer, "nstvout", (*step)["velocityOutputFrequency"].toString());
        writeLine(writer, "nstfout", (*step)["forceOutputFrequency"].toString());
        // TODO make logging freq configurable
        writeLine(writer, "nstlog", QString::number(1000));

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
