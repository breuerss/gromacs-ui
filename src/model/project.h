#ifndef PROJECT_H
#define PROJECT_H


#include <QString>
#include <QDataStream>
#include <QObject>
#include <vector>
#include <memory>
#include "../pipeline/supportedsteps.h"

namespace Model {

class SystemSetup;
class Simulation;

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

  std::shared_ptr<Simulation> addStep();
  void clearSteps();
  void removeStep(int index);
  const std::vector<std::shared_ptr<Simulation>>& getSteps() const;

  Pipeline::StepType addPipelineStep();
  void clearPipelineSteps();
  void removePipelineStep(int index);
  const std::vector<Pipeline::StepType>& getPipelineSteps() const { return pipelineSteps; }

  std::shared_ptr<SystemSetup> getSystemSetup() const;

  QString getProjectPath();

  Q_PROPERTY(QString name MEMBER name NOTIFY nameChanged);

signals:
  void stepAdded(std::shared_ptr<Simulation> step, int at);
  void stepRemoved(std::shared_ptr<Simulation> step, int at);

  void pipelineStepAdded(Pipeline::StepType step, int at);
  void pipelineStepRemoved(Pipeline::StepType step, int at);

  void nameChanged(const QString&);

private:
  std::vector<std::shared_ptr<Simulation>> steps;
  std::shared_ptr<SystemSetup> systemSetup;

  std::vector<Pipeline::StepType> pipelineSteps;
  QString name = "";
};

QDataStream &operator<<(QDataStream &out, const Project &project);
QDataStream &operator>>(QDataStream &in, Project &project);

}

#endif // PROJECT_H
