#ifndef RUNSIMULATION_H
#define RUNSIMULATION_H

#include "executor.h"
#include "../model/project.h"
#include <memory>

namespace Command {

class RunSimulation : public Executor
{
public:
    explicit RunSimulation(std::shared_ptr<Project> project, int stepIndex, QObject *parent = nullptr);
    void exec() override;

private:
    bool execGrompp(
        const QString& mdpFile,
        const QString& inputStructure,
        const QString& topology,
        const QString& output,
        const QString& workingDirectory
        );
    QString getGmx() const;

    std::shared_ptr<Project> project;
    int stepIndex;
};

}

#endif // RUNSIMULATION_H
