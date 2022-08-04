#include "pdbconverter.h"

#include <QProcess>
#include "settings.h"
#include <QDebug>

QString PdbConverter::convert(const QString &fileName, const QStringList &chains)
{
    QString outputFileName = fileName;
    outputFileName.replace(".pdb", "_processed.pdb");

    Settings settings;
    QString command = settings.value(Settings::PDB_CHAIN).toString();
    QStringList arguments;
    arguments << "-" + chains.join(",") << fileName;

    qDebug() << command << arguments;
    QProcess process;
    process.setStandardOutputFile(outputFileName);
    process.start(command, arguments);
    process.waitForFinished();
    return outputFileName;
}
