#ifndef PIPELINE_SMOOTHTRAJECTORY_COMMAND_H
#define PIPELINE_SMOOTHTRAJECTORY_COMMAND_H

#include "../../command/executor.h"
#include <memory>

namespace Pipeline { namespace SmoothTrajectory {

class Command : public ::Command::Executor
{
public:
  explicit Command();
  void doExecute() override;
  bool canExecute() const override;

private:
  QString getInputFileName() const;
};

} }

#endif
