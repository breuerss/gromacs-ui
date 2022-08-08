#include "systemsetup.h"
#include "project.h"
#include "../pdbconverter.h"

#include <QDebug>
#include <QMetaProperty>

namespace Model {

SystemSetup::SystemSetup()
{
  connect(this, &SystemSetup::ionConcentrationChanged, this, &SystemSetup::evaluateConfigReady);
  connect(this, &SystemSetup::forceFieldChanged, this, &SystemSetup::evaluateConfigReady);
  connect(this, &SystemSetup::waterModelChanged, this, &SystemSetup::evaluateConfigReady);
  connect(this, &SystemSetup::boxTypeChanged, this, &SystemSetup::evaluateConfigReady);
  connect(this, &SystemSetup::distanceChanged, this, &SystemSetup::evaluateConfigReady);
  connect(this, &SystemSetup::negativeIonChanged, this, &SystemSetup::evaluateConfigReady);
  connect(this, &SystemSetup::positiveIonChanged, this, &SystemSetup::evaluateConfigReady);

  connect(this, &SystemSetup::removeHeteroAtomsChanged, this, &SystemSetup::filterSourceStructure);

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

bool SystemSetup::getStructureReady() const
{
  return structureReady;
}

void SystemSetup::setStructureReady(bool newStructureReady)
{
  const bool changed = structureReady != newStructureReady;
  structureReady = newStructureReady;
  if (changed)
  {
    emit structureReadyChanged(structureReady);
  }
}

const QString& SystemSetup::getNeutralisedStructureFile() const
{
  return neutralisedStructureFile;
}

void SystemSetup::setNeutralisedStructureFile(const QString& newNeutralisedStructureFile)
{
  neutralisedStructureFile = newNeutralisedStructureFile;

  emit neutralisedStructureFileChanged(neutralisedStructureFile);
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
  const bool newConfigReady = !boxType.isEmpty() &&
    !waterModel.isEmpty() &&
    !forceField.isEmpty() &&
    !filteredStructureFile.isEmpty();

  configReady = newConfigReady;
  emit configReadyChanged(configReady);
}

QDataStream &operator<<(QDataStream &out, const SystemSetup &systemSetup)
{
  const QMetaObject *metaobject = systemSetup.metaObject();
  int count = metaobject->propertyCount();
  for (int i = 0; i < count; ++i) {
    QMetaProperty metaproperty = metaobject->property(i);
    const char *name = metaproperty.name();
    QVariant value = systemSetup.property(name);

    out << value;
  }


  return out;
}

QDataStream &operator>>(QDataStream &in, SystemSetup &systemSetup)
{
  const QMetaObject *metaobject = systemSetup.metaObject();
  int count = metaobject->propertyCount();

  for (int i = 0; i < count; ++i) {
    QMetaProperty metaproperty = metaobject->property(i);
    const char *name = metaproperty.name();

    QVariant value;
    in >> value;
    systemSetup.setProperty(name, value);
  }


  return in;
}

}
