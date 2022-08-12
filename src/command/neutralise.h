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
  explicit Neutralise(
    std::shared_ptr<Model::SystemSetup> newSystemSetup,
    QObject *parent = nullptr
    );
  void doExecute() override;
  QString getOutputFilename() const override;

private:
  const std::shared_ptr<Model::SystemSetup> systemSetup;
};

}

#endif // NEUTRALISE_H
