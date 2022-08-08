#ifndef SOLVATE_H
#define SOLVATE_H

#include "executor.h"
#include <memory>

namespace Model {
class SystemSetup;
}

namespace Command {

class Solvate : public Executor
{
public:
  explicit Solvate(std::shared_ptr<Model::SystemSetup> systemSetup, QObject *parent = nullptr);
  void exec() override;

private:
  const std::shared_ptr<Model::SystemSetup> systemSetup;
  static QString getWaterBoxFor(const QString& solvent);
};

}
#endif // SOLVATE_H
