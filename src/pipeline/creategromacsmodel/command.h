#ifndef PIPELINE_CREATEGROMACSMODEL_COMMAND_H
#define PIPELINE_CREATEGROMACSMODEL_COMMAND_H

#include "../../command/executor.h"
#include <memory>

namespace Model {
  class SystemSetup;
}

namespace Pipeline { namespace CreateGromacsModel {

class Command : public ::Command::Executor
{
public:
    explicit Command();
    void doExecute() override;
    bool canExecute() const override;
    QString getName() const override;

private:
    QString getInputFilename() const;
    QString getOutputFilename() const;
};

} }

#endif
