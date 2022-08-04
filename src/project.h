#ifndef PROJECT_H
#define PROJECT_H

#include "step.h"

class SystemSetup;

#include <QString>
#include <vector>
#include <memory>

class Project
{
public:
    Project(const QString& name);

    void addStep();
    const std::vector<std::shared_ptr<Step>>& getSteps() const;
    std::shared_ptr<SystemSetup> getSystemSetup();
    const QString& getName();
    QString getProjectPath();

private:
    std::vector<std::shared_ptr<Step>> steps;
    std::shared_ptr<SystemSetup> systemSetup;
    QString name;
};

#endif // PROJECT_H
