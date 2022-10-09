#include "pdbinfoextractor.h"

#include "appprovider.h"
#include <QProcess>
#include <QDebug>

QStringList PdbInfoExtractor::getChains(const QString &pdbFile)
{
  QString command = AppProvider::get("pdb_wc");
  QStringList args;
  args << "-c" << pdbFile;
  qDebug() << "executing" << command << args.join(" ");
  QProcess process;
  process.start(command, args);
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
