#ifndef PIPELINE_NEUTRALISE_COMMAND_H
#define PIPELINE_NEUTRALISE_COMMAND_H

#include "../../command/executor.h"

namespace Pipeline { namespace Neutralise {

class Command : public ::Command::Executor
{
public:
  explicit Command() = default;
  void doExecute() override;
  bool canExecute() const override;
  QString getName() const override;

private:
  QString getOutputFilename() const;
  QString getInputFilename() const;
};

} }

#endif
