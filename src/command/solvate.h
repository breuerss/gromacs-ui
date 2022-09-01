#ifndef SOLVATE_H
#define SOLVATE_H

#include "executor.h"
#include "../model/systemsetup.h"
#include "inputoutputfilelink.h"
#include <memory>

namespace Command {

class Solvate : public Executor, public InputOutputFileLink
{
public:
  explicit Solvate(std::shared_ptr<Model::SystemSetup> systemSetup);
  void doExecute() override;
  bool canExecute() const override;
  QString getName() const override;
  QString getOutputFilename() const override;

private:
  const std::shared_ptr<Model::SystemSetup> systemSetup;
  static QString getWaterBoxFor(const Model::SystemSetup::WaterModel& solvent);
};

}
#endif // SOLVATE_H
