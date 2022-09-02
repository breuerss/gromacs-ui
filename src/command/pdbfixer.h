#ifndef PDBFIXER_H
#define PDBFIXER_H

#include "executor.h"
#include "inputoutputfilelink.h"
#include <memory>

namespace Command {

class PdbFixer : public Executor, public InputOutputFileLink
{
public:
  explicit PdbFixer();
  void doExecute() override;
  QString getName() const override;
  QString getOutputFilename() const override;
  bool canExecute() const override;
};

}

#endif // CREATEBOX_H
