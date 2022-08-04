#include "pdbinfoextractor.h"

#include "settings.h"
#include <QProcess>
#include <QDebug>

QStringList PdbInfoExtractor::getChains(const QString &pdbFile)
{
    Settings settings;
    QString command = settings.value(Settings::PDB_WC).toString();
    command += " -c " + pdbFile;
    qDebug() << "executing" << command;
    QProcess process;
    process.start(command);
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QStringList chains;
    QStringList lines = output.split("\n");
    if (lines.length() > 1)
    {
        QStringList results = lines[1].trimmed().split("\t");
        if (results.length() > 1)
        {
            chains = results[1].split(",");
        }
    }
    qDebug() << "chains" << chains;

    return chains;
}
