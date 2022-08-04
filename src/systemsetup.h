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
    void setChains(const QStringList& chains);

    void setSourceStructureFile(const QString& newSourceStructureFile);
    void setProcessedStructureFile(const QString& newProcessedStructureFile);
    void setBoxedStructureFile(const QString& newBoxedStructureFile);
    void setSolvatedStructureFile(const QString& newSolvatedStructureFile);
    void setForceField(const QString& newForceField);
    void setWaterModel(const QString& newWaterModel);
    void setBoxType(const QString& newBoxType);
    void setDistance(double distance);

    const QString& getForceField() const;
    const QString& getWaterModel() const;
    const QString& getBoxType() const;
    const QString& getFilteredStructureFile() const;
    const QString& getProcessedStructureFile() const;
    const QString& getBoxedStructureFile() const;
    const QString& getSolvatedStructureFile() const;
    double getDistance() const;

signals:
    void sourceStructureFileChanged(const QString& sourceStructureFile);
    void filteredStructureFileChanged(const QString& filteredStructureFile);
    void processedStructureFileChanged(const QString& processedStructureFile);
    void boxedStructureFileChanged(const QString& boxedStructureFile);
    void solvatedStructureFileChanged(const QString& solvatedStructureFile);
    void configReady();

private:
    Project* project;
    QString sourceStructureFile;
    QString filteredStructureFile;
    QString processedStructureFile;
    QString boxedStructureFile;
    QString solvatedStructureFile;
    QStringList chains;

    QString waterModel;
    QString boxType;
    QString forceField;
    double distance;

    void evaluateConfigReady();
    void filterSourceStructure();
};

#endif // SYSTEMSETUP_H
