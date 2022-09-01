#ifndef NEUTRALISE_H
#define NEUTRALISE_H

#include "executor.h"
#include "inputoutputfilelink.h"

#include <memory>

namespace Model {
class SystemSetup;
}

namespace Command {

class Neutralise : public Executor, public InputOutputFileLink
{
public:
  explicit Neutralise(std::shared_ptr<Model::SystemSetup> newSystemSetup);
  void doExecute() override;
  bool canExecute() const override;
  QString getName() const override;
  QString getOutputFilename() const override;

private:
  const std::shared_ptr<Model::SystemSetup> systemSetup;
};

}

#endif // NEUTRALISE_H
