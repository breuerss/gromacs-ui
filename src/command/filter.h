#ifndef COMMAND_FILTER_H
#define COMMAND_FILTER_H

#include <QObject>
#include <QStringList>
#include "executor.h"
#include "inputoutputfilelink.h"

namespace Model {
class SystemSetup;
}

namespace Command {

class Filter : public Executor, public InputOutputFileLink
{
public:
  explicit Filter(std::shared_ptr<Model::SystemSetup> systemSetup, QObject *parent = nullptr);

  void doExecute() override;
  QString getName() const override;
  QString getOutputFilename() const override;

private:
  const std::shared_ptr<Model::SystemSetup> systemSetup;
};

}

#endif
