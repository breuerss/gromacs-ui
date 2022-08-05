#include "pdbconverter.h"

#include "settings.h"
#include "statusmessagesetter.h"
#include <QProcess>
#include <QTranslator>
#include <QDebug>
#include <QTemporaryFile>

QString PdbConverter::convert(const QString &fileName,
                              const QStringList &chains,
                              bool removeHeteroAtoms)
{
    QString outputFileName = fileName;
    outputFileName.replace(".pdb", "_filtered.pdb");

    Settings settings;
    QString command = settings.value(Settings::PDB_SELCHAIN).toString();
    if (command.isEmpty())
    {
        StatusMessageSetter::getInstance()->setMessage("Path to pdb_selchain is not set.");
        return QString();
    }
    QStringList arguments;
    arguments << "-" + chains.join(",") << fileName;

    qDebug() << command << arguments;
    QProcess process;
    process.setStandardOutputFile(outputFileName);
    process.start(command, arguments);
    process.waitForFinished();

    if (removeHeteroAtoms)
    {
        QString tmpFileName = getTemporaryFileName();
        QFile(outputFileName).copy(tmpFileName);
        command = settings.value(Settings::PDB_DELHETATM).toString();
        if (command.isEmpty())
        {
            StatusMessageSetter::getInstance()->setMessage("Path to pdb_delhetatm is not set.");
            return QString();
        }

        command += " " + tmpFileName;
        process.start(command);
        process.waitForFinished();
    }
    return outputFileName;
}

QString PdbConverter::getTemporaryFileName()
{
    // hack to get a temporary file name
    // QFile::copy does not write if the file exists
    // Creating an own scope for QTemporaryFile ensures the file
    // is deleted before QFile::copy writes to it.
    QTemporaryFile tmp;
    tmp.open();
    return tmp.fileName();
}
