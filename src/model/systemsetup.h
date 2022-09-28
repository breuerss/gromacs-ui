#ifndef SYSTEMSETUP_H
#define SYSTEMSETUP_H

#include <QString>
#include "serializable.h"

namespace Model {

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
  Q_PROPERTY(bool removeHeteroAtoms MEMBER removeHeteroAtoms NOTIFY removeHeteroAtomsChanged)

signals:
  void pdbCodeChanged(const QString&);

  void removeHeteroAtomsChanged(bool);

  void sourceStructureFileChanged(const QString& sourceStructureFile);
  void processedStructureFileChanged(const QString& processedStructureFile);
  void configReadyChanged(bool ready);

private:
  QString pdbCode;

  QString sourceStructureFile;
  QString processedStructureFile;
  QStringList chains;

  bool removeHeteroAtoms = true;

  void evaluateConfigReady();
};

}


#endif // SYSTEMSETUP_H
