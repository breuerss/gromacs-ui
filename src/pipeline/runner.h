#ifndef PIPELINE_RUNNER_H
#define PIPELINE_RUNNER_H

#include "src/model/project.h"
#include "src/pipeline/step.h"
#include <QObject>
#include <memory>

namespace Pipeline {

class Runner : public QObject
{
  Q_OBJECT
public:
  static Runner* getInstance();
  void startPipeline();
  void stopPipeline();
  bool isRunning();

  using Type = Command::FileObject::Type;
  static QList<std::shared_ptr<Pipeline::Step>>
  getNextStepsFor(
    std::shared_ptr<Pipeline::Step> step,
    std::shared_ptr<Model::Project>,
    const QList<Command::FileObject::Type> relevantTypes = {});

  Q_PROPERTY(bool isRunning READ isRunning NOTIFY runningChanged);

signals:
  void runningChanged(bool running);

private:
  Runner() = default;
  QList<std::shared_ptr<Pipeline::Step>> getStartingSteps(std::shared_ptr<Model::Project>) const;
  void handleNextSteps(std::shared_ptr<Pipeline::Step> step, std::shared_ptr<Model::Project>);
  void cleanUpCommand(std::shared_ptr<Command::Executor> command);

  QMap<std::shared_ptr<Command::Executor>, QMetaObject::Connection> runningSteps;
};

}

#endif
