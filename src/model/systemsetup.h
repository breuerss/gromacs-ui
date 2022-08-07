#ifndef SYSTEMSETUP_H
#define SYSTEMSETUP_H

#include <QString>
#include <QObject>

class Project;

class SystemSetup : public QObject
{
    Q_OBJECT
public:
    SystemSetup(Project* project);
    Project* getProject();
    void useChain(const QString& chain, bool use = true);

    void setPdbCode(const QString& pdbCode);
    void setChains(const QStringList& chains);
    void setSourceStructureFile(const QString& newSourceStructureFile);
    void setFilteredStructureFile(const QString& newFilteredStructureFile);
    void setProcessedStructureFile(const QString& newProcessedStructureFile);
    void setBoxedStructureFile(const QString& newBoxedStructureFile);
    void setSolvatedStructureFile(const QString& newSolvatedStructureFile);
    void setNeutralisedStructureFile(const QString& neutralisedStructureFile);
    void setForceField(const QString& newForceField);
    void setWaterModel(const QString& newWaterModel);
    void setBoxType(const QString& newBoxType);
    void setDistance(double distance);
    void setRemoveHeteroAtoms(bool newRemoveHeteroAtoms);

    void setIonContration(double newConcentation);
    void setPositiveIon(const QString&);
    void setNegativeIon(const QString&);

    void setStructureReady(bool);

    const QString& getPdbCode() const;
    const QStringList& getChains() const;
    const QString& getForceField() const;
    const QString& getWaterModel() const;
    const QString& getBoxType() const;
    const QString& getFilteredStructureFile() const;
    const QString& getProcessedStructureFile() const;
    const QString& getBoxedStructureFile() const;
    const QString& getSolvatedStructureFile() const;
    const QString& getNeutralisedStructureFile() const;
    double getDistance() const;
    bool getRemoveHeteroAtoms() const;

    double getIonContration() const;
    const QString& getPositiveIon() const;
    const QString& getNegativeIon() const;

    bool getStructureReady() const;

signals:
    void sourceStructureFileChanged(const QString& sourceStructureFile);
    void filteredStructureFileChanged(const QString& filteredStructureFile);
    void processedStructureFileChanged(const QString& processedStructureFile);
    void boxedStructureFileChanged(const QString& boxedStructureFile);
    void solvatedStructureFileChanged(const QString& solvatedStructureFile);
    void neutralisedStructureFileChanged(const QString& solvatedStructureFile);
    void configReadyChanged(bool ready);
    void structureReadyChanged(bool);

private:
    Project* project;
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


#endif // SYSTEMSETUP_H
