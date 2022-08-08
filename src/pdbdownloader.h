#ifndef PDBDOWNLOADER_H
#define PDBDOWNLOADER_H

#include <QNetworkAccessManager>

class PdbDownloader : public QNetworkAccessManager
{
  Q_OBJECT
public:
  explicit PdbDownloader(QObject *parent = nullptr);

  void download(const QString& pdbCode, const QString& fileName);

signals:
  void downloaded(const QString& pdbCode, const QString& content);
  void error();
  void notFound();

private:
  QString baseUrl = "https://files.rcsb.org/download/";
  QString createUrlFor(const QString& pdbCode);
};

#endif // PDBDOWNLOADER_H
