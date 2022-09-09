#ifndef PIPELINE_CREATEGROMACSMODEL_COMMAND_H
#define PIPELINE_CREATEGROMACSMODEL_COMMAND_H

#include "../../command/executor.h"

namespace Pipeline { namespace CreateGromacsModel {

class Command : public ::Command::Executor
{
public:
    Command() = default;
    void doExecute() override;
    bool canExecute() const override;
    QString getName() const override;

private:
    QString getInputFilename() const;
    QString getOutputFilename() const;
};

} }

#endif
