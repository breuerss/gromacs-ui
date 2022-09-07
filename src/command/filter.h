#ifndef COMMAND_FILTER_H
#define COMMAND_FILTER_H

#include <QObject>
#include <QStringList>
#include "executor.h"

namespace Model {
class SystemSetup;
}

namespace Command {

class Filter : public Executor
{
public:
  explicit Filter(std::shared_ptr<Model::SystemSetup> systemSetup);

  void doExecute() override;
  bool canExecute() const override;
  QString getName() const override;

private:
  QString getOutputFilename() const;
  const std::shared_ptr<Model::SystemSetup> systemSetup;
};

}

#endif
