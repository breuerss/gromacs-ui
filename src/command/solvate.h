#ifndef SOLVATE_H
#define SOLVATE_H

#include "executor.h"
#include "../model/systemsetup.h"
#include <memory>

namespace Command {

class Solvate : public Executor
{
public:
  explicit Solvate(std::shared_ptr<Model::SystemSetup> systemSetup, QObject *parent = nullptr);
  void doExecute() override;

private:
  const std::shared_ptr<Model::SystemSetup> systemSetup;
  static QString getWaterBoxFor(const Model::SystemSetup::WaterModel& solvent);
};

}
#endif // SOLVATE_H
