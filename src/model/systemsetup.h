#ifndef SYSTEMSETUP_H
#define SYSTEMSETUP_H

#include <QString>
#include <QObject>

namespace Model {

class Project;

class SystemSetup : public QObject
{
  Q_OBJECT
public:
  SystemSetup();
  void useChain(const QString& chain, bool use = true);

  void setStructureReady(bool);

  void setChains(const QStringList& chains);
  void setSourceStructureFile(const QString& newSourceStructureFile);
  void setFilteredStructureFile(const QString& newFilteredStructureFile);
  void setProcessedStructureFile(const QString& newProcessedStructureFile);
  void setBoxedStructureFile(const QString& newBoxedStructureFile);
  void setSolvatedStructureFile(const QString& newSolvatedStructureFile);
  void setNeutralisedStructureFile(const QString& neutralisedStructureFile);

  const QStringList& getChains() const;
  const QString& getFilteredStructureFile() const;
  const QString& getProcessedStructureFile() const;
  const QString& getBoxedStructureFile() const;
  const QString& getSolvatedStructureFile() const;
  const QString& getNeutralisedStructureFile() const;

  bool getStructureReady() const;

  Q_PROPERTY(QString pdbCode MEMBER pdbCode NOTIFY pdbCodeChanged)
  Q_PROPERTY(double ionConcentration MEMBER ionConcentration NOTIFY ionConcentrationChanged)
  Q_PROPERTY(QString negativeIon MEMBER negativeIon NOTIFY negativeIonChanged)
  Q_PROPERTY(QString positiveIon MEMBER positiveIon NOTIFY positiveIonChanged)
  Q_PROPERTY(QString boxType MEMBER boxType NOTIFY boxTypeChanged)
  Q_PROPERTY(QString waterModel MEMBER waterModel NOTIFY waterModelChanged)
  Q_PROPERTY(QString forceField MEMBER forceField NOTIFY forceFieldChanged)
  Q_PROPERTY(double distance MEMBER distance NOTIFY distanceChanged)
  Q_PROPERTY(bool removeHeteroAtoms MEMBER removeHeteroAtoms NOTIFY removeHeteroAtomsChanged)

signals:
  void pdbCodeChanged(const QString&);
  void boxTypeChanged(const QString&);
  void waterModelChanged(const QString&);
  void forceFieldChanged(const QString&);
  void negativeIonChanged(const QString&);
  void positiveIonChanged(const QString&);

  void ionConcentrationChanged(double);
  void distanceChanged(double);

  void removeHeteroAtomsChanged(bool);

  void sourceStructureFileChanged(const QString& sourceStructureFile);
  void filteredStructureFileChanged(const QString& filteredStructureFile);
  void processedStructureFileChanged(const QString& processedStructureFile);
  void boxedStructureFileChanged(const QString& boxedStructureFile);
  void solvatedStructureFileChanged(const QString& solvatedStructureFile);
  void neutralisedStructureFileChanged(const QString& solvatedStructureFile);
  void configReadyChanged(bool ready);
  void structureReadyChanged(bool);

private:
  QString pdbCode;

  QString sourceStructureFile;
  QString filteredStructureFile;
  QString processedStructureFile;
  QString boxedStructureFile;
  QString solvatedStructureFile;
  QString neutralisedStructureFile;
  QString positiveIon = "NA";
  QString negativeIon = "CL";
  double ionConcentration = 0.15;
  QStringList chains;

  QString waterModel = "tip3p";
  QString boxType = "dodecahedron";
  QString forceField = "oplsaa";
  double distance = 1.0;
  bool removeHeteroAtoms = true;
  bool configReady = false;
  bool structureReady = false;

  void evaluateConfigReady();
  void filterSourceStructure();
};

QDataStream &operator<<(QDataStream &out, const SystemSetup &project);
QDataStream &operator>>(QDataStream &in, SystemSetup &project);

}


#endif // SYSTEMSETUP_H
