#ifndef PDBINFOEXTRACTOR_H
#define PDBINFOEXTRACTOR_H

#include <QStringList>

class PdbInfoExtractor
{
public:
    static QStringList getChains(const QString& pdbFile);
    static QStringList getHetatms(const QString& pdbFile);
};

#endif // PDBINFOEXTRACTOR_H
