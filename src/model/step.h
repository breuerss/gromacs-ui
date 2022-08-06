#ifndef STEP_H
#define STEP_H

#include <QVariantMap>
#include <QString>
#include <QObject>

class Step : public QObject, public QVariantMap
{
    Q_OBJECT
public:
    Step();
    QString getName() const;
    QString getDirectory() const;
    QVariant& operator[](const QString& key);
    QVariant operator[](const QString& key) const;

signals:
    void nameChanged();
    void directoryChanged();
};

#endif // STEP_H
