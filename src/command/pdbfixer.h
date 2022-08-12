#ifndef PDBFIXER_H
#define PDBFIXER_H

#include "executor.h"
#include "inputoutputfilelink.h"
#include <memory>

namespace Model {
class SystemSetup;
}

namespace Command {

class PdbFixer : public Executor, public InputOutputFileLink
{
public:
  explicit PdbFixer(QObject *parent = nullptr);
  void doExecute() override;
  QString getOutputFilename() const override;
};

}

#endif // CREATEBOX_H
