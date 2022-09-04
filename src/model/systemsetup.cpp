#include "systemsetup.h"
#include "project.h"

#include <QDebug>
#include <QMetaProperty>

namespace Model {

SystemSetup::SystemSetup()
{
  static bool registered = false;
  if (!registered)
  {
    registered = true;
  }
  connect(this, &SystemSetup::ionConcentrationChanged, this, &SystemSetup::evaluateConfigReady);
//  connect(this, &SystemSetup::forceFieldChanged, this, &SystemSetup::evaluateConfigReady);
//  connect(this, &SystemSetup::waterModelChanged, this, &SystemSetup::evaluateConfigReady);
  //connect(this, &SystemSetup::boxTypeChanged, this, &SystemSetup::evaluateConfigReady);
  //connect(this, &SystemSetup::distanceChanged, this, &SystemSetup::evaluateConfigReady);
  connect(this, &SystemSetup::negativeIonChanged, this, &SystemSetup::evaluateConfigReady);
  connect(this, &SystemSetup::positiveIonChanged, this, &SystemSetup::evaluateConfigReady);
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

const QString& SystemSetup::getSourceStructureFile() const
{
  return sourceStructureFile;
}

void SystemSetup::setProcessedStructureFile(const QString &newProcessedStructureFile)
{
  processedStructureFile = newProcessedStructureFile;
  emit processedStructureFileChanged(processedStructureFile);
  evaluateConfigReady();
}

void SystemSetup::setChains(const QStringList &newChains)
{
  chains = newChains;
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
}

const QString& SystemSetup::getProcessedStructureFile() const
{
  return processedStructureFile;
}

void SystemSetup::evaluateConfigReady()
{
  qDebug() << "evaluateConfigReady"
//    << boxType
//    << waterModel
//    << forceField
    << processedStructureFile;
  emit configReadyChanged(configReady());
}

bool SystemSetup::configReady() const
{
  return //(boxType != BoxType::None) &&
    //(forceField != ForceField::None) &&
    !processedStructureFile.isEmpty();
}

}
