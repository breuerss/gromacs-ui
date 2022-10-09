#ifndef PIPELINE_GYRATE_COMMAND_H
#define PIPELINE_GYRATE_COMMAND_H

#include "../../command/executor.h"
#include "../../command/fileobject.h"

namespace Pipeline { namespace Gyrate {

class Command : public ::Command::Executor
{
public:
  Command() = default;
  void doExecute() override;
  bool canExecute() const override;

private:
  QString getInputFilename() const;
  using Type = ::Command::FileObject::Type;
};

} }

#endif
