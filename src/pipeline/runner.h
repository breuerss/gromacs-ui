#ifndef PIPELINE_RUNNER_H
#define PIPELINE_RUNNER_H

#include "src/model/project.h"
#include "src/pipeline/step.h"
#include <QObject>
#include <memory>

namespace Pipeline {

class Runner : public QObject
{
public:
  static Runner* getInstance();
  void startPipeline();

  using Type = Command::FileObject::Type;
  static QList<std::shared_ptr<Pipeline::Step>>
  getNextStepsFor(
    std::shared_ptr<Pipeline::Step> step,
    std::shared_ptr<Model::Project>,
    const QList<Command::FileObject::Type> relevantTypes = {});

private:
  Runner() = default;
  QList<std::shared_ptr<Pipeline::Step>> getStartingSteps(std::shared_ptr<Model::Project>) const;
  void handleNextSteps(std::shared_ptr<Pipeline::Step> step, std::shared_ptr<Model::Project>);

  QList<QMetaObject::Connection> conns;
  int running = 0;
};

}

#endif
