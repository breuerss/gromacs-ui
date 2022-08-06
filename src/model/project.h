#ifndef PROJECT_H
#define PROJECT_H

#include "step.h"

class SystemSetup;

#include <QString>
#include <QDataStream>
#include <QObject>
#include <vector>
#include <memory>

class Project : public QObject
{
    Q_OBJECT
public:
    Project(const QString& name);

    void addStep();
    void removeStep(int index);
    const std::vector<std::shared_ptr<Step>>& getSteps() const;
    std::shared_ptr<SystemSetup> getSystemSetup() const;

    const QString& getName() const;
    void setName(const QString& newName);
    QString getProjectPath();

signals:
    void stepAdded(std::shared_ptr<Step> step, int at);
    void stepRemoved(std::shared_ptr<Step> step, int at);

private:
    std::vector<std::shared_ptr<Step>> steps;
    std::shared_ptr<SystemSetup> systemSetup;
    QString name;
};

QDataStream &operator<<(QDataStream &out, const Project &project);
QDataStream &operator>>(QDataStream &in, Project &project);

#endif // PROJECT_H
