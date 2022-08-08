#include "pdbdownloader.h"
#include <QNetworkReply>
#include <QDebug>
#include <QFile>

PdbDownloader::PdbDownloader(QObject *parent)
    : QNetworkAccessManager{parent}
{

}

void PdbDownloader::download(const QString& pdbCode, const QString& fileName)
{
    qDebug() << "pdb code" << pdbCode;
    QNetworkRequest request(createUrlFor(pdbCode));
    auto* reply = get(request);
    connect(reply, &QNetworkReply::finished, [this, pdbCode, reply, fileName] () {
        reply->deleteLater();

        if (reply->error() == QNetworkReply::ContentNotFoundError)
        {
            emit notFound();
            return;
        }

        if (reply->error() == QNetworkReply::NoError)
        {
            qDebug() << "pdbCode" << pdbCode;
            QString content = reply->readAll();
            QFile file(fileName);
            file.open(QFile::WriteOnly);
            file.write(content.toUtf8());
            file.close();
            emit downloaded(pdbCode, fileName);
            return;
        }

        qDebug() << reply->error();
        emit error();
    });
}

QString PdbDownloader::createUrlFor(const QString &pdbCode)
{
    return baseUrl + pdbCode + ".pdb1";
}
