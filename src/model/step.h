#ifndef STEP_H
#define STEP_H

#include <QVariantMap>
#include <QString>

class Step : public QVariantMap
{
public:
    Step();
    QString getName() const;
};

#endif // STEP_H
