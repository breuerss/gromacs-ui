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

private:
  using Type = ::Command::FileObject::Type;
  QString getInputFilename() const;
  QPair<std::shared_ptr<QProcess>, QString> getPreparationCommand();
  void neutralise(const QString& ionsTprPath);
};

} }

#endif
