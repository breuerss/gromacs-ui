#ifndef PROJECT_H
#define PROJECT_H


#include "stepvector.h"
#include "../pipeline/simulation/configuration.h"

#include <QString>
#include <QDebug>
#include <QObject>
#include <vector>
#include <memory>

namespace Pipeline {
class Step;
namespace Simulation {
class Step;
}
}

namespace Model {

class Project : public Serializable, public std::enable_shared_from_this<Project>
{
  Q_OBJECT
  typedef std::shared_ptr<Pipeline::Step> StepPointer;
  typedef StepVector StepPointerVector;

public:
  Project();

  template<typename T>
  std::shared_ptr<T> addStep()
  {
    auto step = std::make_shared<T>(shared_from_this());

    auto pipelineStep = std::dynamic_pointer_cast<Pipeline::Step>(step);
    addStep(std::move(pipelineStep));
    return step;
  }

  void addStep(StepPointer&& step);

  void clearSteps();
  void removeStep(int index);
  void removeStep(std::shared_ptr<Pipeline::Step> step);
  const StepPointerVector& getSteps() const;
  StepPointerVector& getSteps();

  QString getProjectPath();
  bool initProjectDir(const QString& subDir = "");

  void createDefaultSimulationSetup();
  QList<std::shared_ptr<Pipeline::Step>> addPreparationPipeline();
  QList<std::shared_ptr<Pipeline::Step>> addSimulationPipeline();

  std::shared_ptr<Pipeline::Simulation::Step>
  addSimulation(Pipeline::Simulation::Configuration::Type type);

  Q_PROPERTY(QString name MEMBER name NOTIFY nameChanged);

signals:
  void stepAdded(StepPointer step, int at);
  void stepRemoved(StepPointer step, int at);

  void nameChanged(const QString&);

private:
  StepPointerVector pipelineSteps;
  QString name = "";
};

QJsonObject &operator<<(QJsonObject &out, const std::shared_ptr<Project> project);
QJsonObject &operator>>(QJsonObject &in, std::shared_ptr<Project> project);

}

#endif // PROJECT_H
