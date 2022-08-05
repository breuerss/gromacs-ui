#ifndef PDBCONVERTER_H
#define PDBCONVERTER_H

#include <QStringList>

class PdbConverter
{
public:
    static QString convert(
            const QString& fileName,
            const QStringList& chains = {},
            bool removeHeteroAtoms = false);
    static QString getTemporaryFileName();
};

#endif // PDBCONVERTER_H
