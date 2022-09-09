#ifndef PIPELINE_CREATEBOX_COMMAND_H
#define PIPELINE_CREATEBOX_COMMAND_H

#include "../../command/executor.h"

namespace Pipeline { namespace CreateBox {

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
