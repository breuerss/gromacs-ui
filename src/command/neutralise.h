#ifndef NEUTRALISE_H
#define NEUTRALISE_H

#include "executor.h"

#include <memory>

namespace Model {
class SystemSetup;
}

namespace Command {

class Neutralise : public Executor
{
public:
  explicit Neutralise(
    std::shared_ptr<Model::SystemSetup> newSystemSetup,
    QObject *parent = nullptr
    );
  void exec() override;

private:
  const std::shared_ptr<Model::SystemSetup> systemSetup;
};

}

#endif // NEUTRALISE_H
