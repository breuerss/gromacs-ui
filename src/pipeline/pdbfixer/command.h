#ifndef PIPELINE_PDBFIXER_COMMAND_H
#define PIPELINE_PDBFIXER_COMMAND_H

#include "../../command/executor.h"

namespace Pipeline { namespace PdbFixer {

class Command : public ::Command::Executor
{
public:
  Command() = default;
  void doExecute() override;
  bool canExecute() const override;

private:
  QString getInputFilename() const;
};

} }

#endif
