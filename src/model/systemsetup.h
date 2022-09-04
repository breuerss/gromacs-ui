#ifndef SYSTEMSETUP_H
#define SYSTEMSETUP_H

#include <QString>
#include "serializable.h"

namespace Model {

class Project;

class SystemSetup : public Serializable
{
  Q_OBJECT
public:
  SystemSetup();

  void useChain(const QString& chain, bool use = true);

  void setChains(const QStringList& chains);
  void setSourceStructureFile(const QString& newSourceStructureFile);
  void setProcessedStructureFile(const QString& newProcessedStructureFile);
  bool configReady() const;

  const QStringList& getChains() const;
  const QString& getSourceStructureFile() const;
  const QString& getProcessedStructureFile() const;

  Q_PROPERTY(QString pdbCode MEMBER pdbCode NOTIFY pdbCodeChanged)
  Q_PROPERTY(double ionConcentration MEMBER ionConcentration NOTIFY ionConcentrationChanged)
  Q_PROPERTY(QString negativeIon MEMBER negativeIon NOTIFY negativeIonChanged)
  Q_PROPERTY(QString positiveIon MEMBER positiveIon NOTIFY positiveIonChanged)
  Q_PROPERTY(bool removeHeteroAtoms MEMBER removeHeteroAtoms NOTIFY removeHeteroAtomsChanged)

signals:
  void pdbCodeChanged(const QString&);
  void negativeIonChanged(const QString&);
  void positiveIonChanged(const QString&);

  void ionConcentrationChanged(double);

  void removeHeteroAtomsChanged(bool);

  void sourceStructureFileChanged(const QString& sourceStructureFile);
  void processedStructureFileChanged(const QString& processedStructureFile);
  void configReadyChanged(bool ready);

private:
  QString pdbCode;

  QString sourceStructureFile;
  QString processedStructureFile;
  QString positiveIon = "NA";
  QString negativeIon = "CL";
  double ionConcentration = 0.15;
  QStringList chains;

  bool removeHeteroAtoms = true;

  void evaluateConfigReady();
};

}


#endif // SYSTEMSETUP_H
