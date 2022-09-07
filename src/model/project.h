#ifndef PROJECT_H
#define PROJECT_H


#include <QString>
#include <QDebug>
#include <QDataStream>
#include <QObject>
#include <vector>
#include <memory>
#include "../pipeline/step.h"

namespace Config {
class Simulation;
}

namespace Model {

class SystemSetup;

class Project : public QObject, public std::enable_shared_from_this<Project>
{
  Q_OBJECT
  typedef std::shared_ptr<Pipeline::Step> StepPointer;
  typedef std::vector<StepPointer> StepPointerVector;

public:
  Project();

  /**
   * TODO
   *
   * Base class
   * Step {
   *    FileObjectProvider* provider
   *    FileObjectConsumer* consumer
   *    ConfigurationType* config
   *    std::string name = pure virtual
   *    showConfigUI() on click;
   *    exec() on play click in node;
   * }
   *
   */

  template<typename T>
  std::shared_ptr<T> addStep()
  {
    auto step = std::make_shared<T>(shared_from_this());

    auto pipelineStep = std::dynamic_pointer_cast<Pipeline::Step>(step);
    pipelineSteps.push_back(pipelineStep);
    emit stepAdded(pipelineStep, pipelineSteps.size() - 1);
    return step;
  }

  void addStep(StepPointer&& step);

  void clearSteps();
  void removeStep(int index);
  const StepPointerVector& getSteps() const;

  std::shared_ptr<SystemSetup> getSystemSetup() const;

  QString getProjectPath();
  bool initProjectDir(const QString& subDir = "");

  Q_PROPERTY(QString name MEMBER name NOTIFY nameChanged);

signals:
  void stepAdded(StepPointer step, int at);
  void stepRemoved(StepPointer step, int at);

  void nameChanged(const QString&);

private:
  std::shared_ptr<SystemSetup> systemSetup;

  StepPointerVector pipelineSteps;
  QString name = "";
};

QDataStream &operator<<(QDataStream &out, const Project &project);
QDataStream &operator>>(QDataStream &in, Project &project);

}

#endif // PROJECT_H
