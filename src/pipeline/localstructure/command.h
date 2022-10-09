#ifndef COMMAND_DOWNLOADPDB_H
#define COMMAND_DOWNLOADPDB_H

#include "../../command/executor.h"
#include <memory>

namespace Pipeline { namespace LocalStructure {

class Command : public ::Command::Executor
{
public:
  explicit Command();
  void doExecute() override;
  bool canExecute() const override;

private:
  QString getFileName() const;
  QMetaObject::Connection configConnection;
};

} }

#endif
