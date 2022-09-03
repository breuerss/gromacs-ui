#ifndef PIPELINE_PDBFIXER_COMMAND_H
#define PIPELINE_PDBFIXER_COMMAND_H

#include "../../command/executor.h"

namespace Pipeline { namespace PdbFixer {

class Command : public ::Command::Executor
{
public:
  Command() = default;
  void doExecute() override;
  QString getName() const override;
  bool canExecute() const override;

private:
  QString getInputFilename() const;
  QString getOutputFilename() const;
};

} }

#endif
