#ifndef PROJECT_H
#define PROJECT_H


#include <QString>
#include <QDataStream>
#include <QObject>
#include <vector>
#include <memory>
#include "../pipeline/supportedsteps.h"

namespace Config {
class Simulation;
}

namespace Model {

class SystemSetup;

class Project : public QObject
{
  Q_OBJECT
public:
  Project();

  /**
   * TODO
   *
   * Introduce std::variant<Step specialisations>
   * using StepType = std::variant<PdbDownloader, Simulation, BoxCreator, TrajectoryViewer..>;
   * using ConfigurationType = std::variant<PdbDownloaderConfig, SimulationConfig, BoxConfig>;
   * GetUIForConfigurtionVisitor {
   * }
   * using ConfigurationViewType = std::variant<UI::PdbDownloaderConfig, UI::SimulationConfig, UI::BoxConfig>;
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
   * introduce std::vector<StepType> pipeline;
   *
   * looping through steps with
   *
   * for (auto& step: pipeline)
   * {
   *    std::visit([](auto&& arg) {
   *       - use file object consumer to create input nodes
   *       - use file object provider to create output nodes
   *       - connect on click signal to create configuration view for configuration
   *         object of step
   *    }, step);
   * }
   *
   * Move Model::Simulation to Configuration namespace
   *
   **/

  template<typename T>
  Pipeline::StepType addStep()
  {
    std::shared_ptr<Project> project;
    project.reset(this);

    auto pipelineStep = std::make_shared<T>(project);
    pipelineSteps.push_back(pipelineStep);
    emit stepAdded(pipelineStep, pipelineSteps.size() - 1);
    return pipelineStep;
  }

  void clearSteps();
  void removeStep(int index);
  const std::vector<Pipeline::StepType>& getSteps() const;

  std::shared_ptr<SystemSetup> getSystemSetup() const;

  QString getProjectPath();

  Q_PROPERTY(QString name MEMBER name NOTIFY nameChanged);

signals:
  void stepAdded(Pipeline::StepType step, int at);
  void stepRemoved(Pipeline::StepType step, int at);

  void nameChanged(const QString&);

private:
  std::shared_ptr<SystemSetup> systemSetup;

  std::vector<Pipeline::StepType> pipelineSteps;
  QString name = "";
};

QDataStream &operator<<(QDataStream &out, const Project &project);
QDataStream &operator>>(QDataStream &in, Project &project);

}

#endif // PROJECT_H
