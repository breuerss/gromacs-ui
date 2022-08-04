#ifndef SYSTEMSETUP_H
#define SYSTEMSETUP_H

#include "project.h"
#include <QString>
#include <QObject>

class SystemSetup : public QObject
{
    Q_OBJECT
public:
    SystemSetup(Project* project);
    Project* getProject();
    void setSourceStructureFile(const QString& newSourceStructureFile);
    void useChain(const QString& chain, bool use = true);

signals:
    void sourceStructureFileChanged(const QString& sourceStructureFile);
    void processedStructureFileChanged(const QString& processedStructureFile);

private:
    Project* project;
    QString sourceStructureFile;
    QString processedStructureFile;
    QStringList chains;
};

#endif // SYSTEMSETUP_H
