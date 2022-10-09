#ifndef PIPELINE_TEMPERATURE_COMMAND_H
#define PIPELINE_TEMPERATURE_COMMAND_H

#include "../../command/executor.h"
#include "../../command/fileobject.h"

namespace Pipeline { namespace Temperature {

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
