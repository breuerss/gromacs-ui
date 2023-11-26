#ifndef PIPELINE_SOLVATE_COMMAND_H
#define PIPELINE_SOLVATE_COMMAND_H

#include "../src/command/executor.h"

namespace Pipeline { namespace Solvate {

class Command : public ::Command::Executor
{
public:
  explicit Command() = default;
  void doExecute() override;
  bool canExecute() const override;

private:
  using Type = ::Command::FileObject::Type;
  QString getInputFilename() const;
  QString getWaterSolventFromTopology();
  static QString getWaterBoxFor(const QString& solvent);
};

} }

#endif
