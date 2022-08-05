#include "systemsetup.h"
#include "project.h"
#include "pdbconverter.h"

#include <QDebug>

SystemSetup::SystemSetup(Project* parent)
    : project(parent)
{

}

Project* SystemSetup::getProject()
{
    return project;
}

void SystemSetup::setSourceStructureFile(const QString &newSourceStructureFile)
{
    const bool changed = sourceStructureFile != newSourceStructureFile;
    sourceStructureFile = newSourceStructureFile;
    if (changed)
    {
        emit sourceStructureFileChanged(sourceStructureFile);
    }
}

void SystemSetup::setFilteredStructureFile(const QString &newFilteredStructureFile)
{
    const bool changed = filteredStructureFile != newFilteredStructureFile;
    filteredStructureFile = newFilteredStructureFile;
    if (changed)
    {
        emit filteredStructureFileChanged(filteredStructureFile);
    }
}

void SystemSetup::setProcessedStructureFile(const QString &newProcessedStructureFile)
{
    const bool changed = processedStructureFile != newProcessedStructureFile;
    processedStructureFile = newProcessedStructureFile;

    if (changed)
    {
        emit processedStructureFileChanged(processedStructureFile);
    }
}

void SystemSetup::setBoxedStructureFile(const QString &newBoxedStructureFile)
{
    const bool changed = boxedStructureFile != newBoxedStructureFile;
    boxedStructureFile = newBoxedStructureFile;

    if (changed)
    {
        emit boxedStructureFileChanged(boxedStructureFile);
    }
}

void SystemSetup::setSolvatedStructureFile(const QString& newSolvatedStructureFile)
{
    const bool changed = solvatedStructureFile != newSolvatedStructureFile;
    qDebug() << "Setting solvated structure file to" << newSolvatedStructureFile << solvatedStructureFile << changed;
    solvatedStructureFile = newSolvatedStructureFile;

    emit solvatedStructureFileChanged(solvatedStructureFile);
}

void SystemSetup::setChains(const QStringList &newChains)
{
    chains = newChains;
    filterSourceStructure();
}

const QStringList& SystemSetup::getChains() const
{
    return chains;
}

void SystemSetup::useChain(const QString& chain, bool use)
{
    qDebug() << chain << use;
    if (use)
    {
        if (!chains.contains(chain))
        {
            chains.append(chain);
        }
    }
    else
    {
        chains.removeAll(chain);
    }

    qDebug() << "chains to use" << chains;
    filterSourceStructure();
}

void SystemSetup::filterSourceStructure()
{
    PdbConverter converter;
    filteredStructureFile = converter.convert(sourceStructureFile, chains, removeHeteroAtoms);
    emit filteredStructureFileChanged(filteredStructureFile);
    evaluateConfigReady();
}

void SystemSetup::setForceField(const QString& newForceField)
{
    qDebug() << "Setting force field to" << newForceField;
    forceField = newForceField;
    evaluateConfigReady();
}

void SystemSetup::setWaterModel(const QString& newWaterModel)
{
    qDebug() << "Setting water model to" << newWaterModel;
    waterModel = newWaterModel;
    evaluateConfigReady();
}

void SystemSetup::setBoxType(const QString& newBoxType)
{
    qDebug() << "Setting box type to" << newBoxType;
    boxType = newBoxType;
    evaluateConfigReady();
}

void SystemSetup::setDistance(double newDistance)
{
    distance = newDistance;
}

void SystemSetup::setRemoveHeteroAtoms(bool newRemoveHeteroAtoms)
{
    removeHeteroAtoms = newRemoveHeteroAtoms;
    filterSourceStructure();
}

const QString& SystemSetup::getForceField() const
{
    return forceField;
}

const QString& SystemSetup::getWaterModel() const
{
    return waterModel;
}

const QString& SystemSetup::getBoxType() const
{
    return boxType;
}

double SystemSetup::getDistance() const
{
    return distance;
}

bool SystemSetup::getRemoveHeteroAtoms() const
{
    return removeHeteroAtoms;
}

const QString& SystemSetup::getFilteredStructureFile() const
{
    return filteredStructureFile;
}

const QString& SystemSetup::getProcessedStructureFile() const
{
    return processedStructureFile;
}

const QString& SystemSetup::getBoxedStructureFile() const
{
    return boxedStructureFile;
}

const QString& SystemSetup::getSolvatedStructureFile() const
{
    return solvatedStructureFile;
}

void SystemSetup::evaluateConfigReady()
{
    qDebug() << "evaluateConfigReady"  << boxType << waterModel << forceField << filteredStructureFile;
    if (
            !boxType.isEmpty() &&
            !waterModel.isEmpty() &&
            !forceField.isEmpty() &&
            !filteredStructureFile.isEmpty())
    {
        emit configReady();
    }
}

QDataStream &operator<<(QDataStream &out, const SystemSetup &systemSetup)
{
    out << systemSetup.getChains()
        << systemSetup.getForceField()
        << systemSetup.getWaterModel()
        << systemSetup.getBoxType()
        << systemSetup.getFilteredStructureFile()
        << systemSetup.getProcessedStructureFile()
        << systemSetup.getBoxedStructureFile()
        << systemSetup.getSolvatedStructureFile()
        << systemSetup.getDistance()
        << systemSetup.getRemoveHeteroAtoms();

    return out;
}

QDataStream &operator>>(QDataStream &in, SystemSetup &systemSetup)
{
    QStringList chains;
    in >> chains;
    systemSetup.setChains(chains);

    QString forceField;
    in >> forceField;
    systemSetup.setForceField(forceField);

    QString waterModel;
    in >> waterModel;
    systemSetup.setWaterModel(waterModel);

    QString boxType;
    in >> boxType;
    systemSetup.setBoxType(boxType);

    QString filteredStructureFile;
    in >> filteredStructureFile;
    systemSetup.setFilteredStructureFile(filteredStructureFile);

    QString processedStructureFile;
    in >> processedStructureFile;
    systemSetup.setProcessedStructureFile(processedStructureFile);

    QString boxedStructureFile;
    in >> boxedStructureFile;
    systemSetup.setBoxedStructureFile(boxedStructureFile);

    QString solvatedStructureFile;
    in >> solvatedStructureFile;
    systemSetup.setSolvatedStructureFile(solvatedStructureFile);

    double distance;
    in >> distance;
    systemSetup.setDistance(distance);

    bool removeHeteroAtoms;
    in >> removeHeteroAtoms;
    systemSetup.setRemoveHeteroAtoms(removeHeteroAtoms);

    return in;
}

