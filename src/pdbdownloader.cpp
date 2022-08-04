#include "pdbdownloader.h"
#include <QNetworkReply>
#include <QDebug>
PdbDownloader::PdbDownloader(QObject *parent)
    : QNetworkAccessManager{parent}
{

}

void PdbDownloader::download(const QString &pdbCode)
{
    qDebug() << "pdb code" << pdbCode;
    QNetworkRequest request(createUrlFor(pdbCode));
    auto* reply = get(request);
    connect(reply, &QNetworkReply::finished, [this, pdbCode, reply] () {
        // TODO error handling
        qDebug() << "pdbCode" << pdbCode;
        QString content = reply->readAll();
        emit downloaded(pdbCode, content);
        reply->deleteLater();
    });
}

QString PdbDownloader::createUrlFor(const QString &pdbCode)
{
    return baseUrl + pdbCode + ".pdb1";
}
