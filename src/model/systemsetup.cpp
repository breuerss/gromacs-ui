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
    qRegisterMetaType<WaterModel>("SystemSetup::WaterModel");
    qRegisterMetaTypeStreamOperators<int>("SystemSetup::WaterModel");
    qRegisterMetaType<BoxType>("SystemSetup::BoxType");
    qRegisterMetaTypeStreamOperators<int>("SystemSetup::BoxType");
    qRegisterMetaType<ForceField>("SystemSetup::ForceField");
    qRegisterMetaTypeStreamOperators<int>("SystemSetup::ForceField");
    registered = true;
  }
  connect(this, &SystemSetup::ionConcentrationChanged, this, &SystemSetup::evaluateConfigReady);
  connect(this, &SystemSetup::forceFieldChanged, this, &SystemSetup::evaluateConfigReady);
  connect(this, &SystemSetup::waterModelChanged, this, &SystemSetup::evaluateConfigReady);
  connect(this, &SystemSetup::boxTypeChanged, this, &SystemSetup::evaluateConfigReady);
  connect(this, &SystemSetup::distanceChanged, this, &SystemSetup::evaluateConfigReady);
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
    << boxType
    << waterModel
    << forceField
    << processedStructureFile;
  emit configReadyChanged(configReady());
}

bool SystemSetup::configReady() const
{
  return (boxType != BoxType::None) &&
    (forceField != ForceField::None) &&
    !processedStructureFile.isEmpty();
}

QString toString(SystemSetup::WaterModel type)
{
  const static QMap<SystemSetup::WaterModel, QString> map = {
    { SystemSetup::WaterModel::SPC, "spc" },
    { SystemSetup::WaterModel::TIP3P, "tip3p" },
    { SystemSetup::WaterModel::TIP4P, "tip4p" },
    { SystemSetup::WaterModel::TIP5P, "tip5p" },
  };

  return map[type];
}

QString toString(SystemSetup::BoxType type)
{
  const static QMap<SystemSetup::BoxType, QString> map = {
    { SystemSetup::BoxType::Cubic, "cubic" },
    { SystemSetup::BoxType::Octahedron, "octahedron" },
    { SystemSetup::BoxType::Dodecahedron, "dodecahedron" },
  };

  return map[type];
}

QString toString(SystemSetup::ForceField type)
{
  const static QMap<SystemSetup::ForceField, QString> map = {
    { SystemSetup::ForceField::CHARMM27, "charmm27" },
    { SystemSetup::ForceField::OPLSAA, "oplsaa" },
  };

  return map[type];
}

}
