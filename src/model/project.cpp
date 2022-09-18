#include "project.h"
#include "../pipeline/simulation/configuration.h"
#include "systemsetup.h"
#include "../settings.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <memory>
#include <QJsonObject>
#include <QJsonArray>
#include "../pipeline/stepfactory.h"
#include "../command/fileobjectconsumer.h"
#include "../command/fileobjectprovider.h"
#include "../pipeline/steps.h"

namespace Model {

Project::Project()
  : QObject(nullptr)
{
}

void Project::addStep(StepPointer&& step)
{
  pipelineSteps.push_back(step);
  emit stepAdded(step, pipelineSteps.size() - 1);
}

void Project::removeStep(int at)
{
  auto step = pipelineSteps[at];
  pipelineSteps.erase(pipelineSteps.begin() + at);
  emit stepRemoved(step, at);
}

void Project::removeStep(std::shared_ptr<Pipeline::Step> step)
{
  auto it = std::remove(pipelineSteps.begin(), pipelineSteps.end(), step);
  int at = std::distance(pipelineSteps.begin(), it);
  pipelineSteps.erase(it, pipelineSteps.end());
  emit stepRemoved(step, at);
}

void Project::clearSteps()
{
  while (pipelineSteps.size())
  {
    removeStep(0);
  }
}

const Project::StepPointerVector& Project::getSteps() const
{
  return pipelineSteps;
}

QString Project::getProjectPath()
{
  Settings settings;
  auto defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/gromacs-ui";
  auto projectsPath = settings.value(Settings::PROJECTS_DIRECTORY, defaultPath).toString();
  return projectsPath + "/" + name;
}

bool Project::initProjectDir(const QString& subDir)
{
  QDir dir(getProjectPath());
  return dir.mkpath(subDir);
}

QList<std::shared_ptr<Pipeline::Step>> Project::addPreparationPipeline()
{
  auto pdbFixer = addStep<Pipeline::PdbFixer::Step>();
  auto gromacsModel = addStep<Pipeline::CreateGromacsModel::Step>();
  gromacsModel->getFileObjectConsumer()->connectTo(
    pdbFixer->getFileObjectProvider()->provides()[0]
    );
  auto createBox = addStep<Pipeline::CreateBox::Step>();
  createBox->getFileObjectConsumer()->connectTo(
    gromacsModel->getFileObjectProvider()->provides()[0]
    );

  auto solvate = addStep<Pipeline::Solvate::Step>();
  solvate->getFileObjectConsumer()->connectTo(
    createBox->getFileObjectProvider()->provides()[0]
    );

  auto neutralise = addStep<Pipeline::Neutralise::Step>();
  neutralise->getFileObjectConsumer()->connectTo(
    solvate->getFileObjectProvider()->provides()[0]
    );

  return { pdbFixer, gromacsModel, createBox, solvate, neutralise };
}

QList<std::shared_ptr<Pipeline::Step>> Project::addSimulationPipeline()
{
  using Type = Pipeline::Simulation::Configuration::Type;
  auto minimisation = addSimulation(Type::Minimisation);
  auto equillibration = addSimulation(Type::NVT);
  equillibration->getFileObjectConsumer()->connectTo(
    minimisation->getFileObjectProvider()->provides()[0]
    );

  auto simulation = addSimulation(Type::NPT);
  simulation->getFileObjectConsumer()->connectTo(
    equillibration->getFileObjectProvider()->provides()[0]
    );

  return { minimisation, equillibration, simulation };
}

std::shared_ptr<Pipeline::Simulation::Step>
Project::addSimulation(Pipeline::Simulation::Configuration::Type type)
{
  auto step = addStep<Pipeline::Simulation::Step>();
  step->getConfiguration()
    ->setProperty("simulationType", QVariant::fromValue(type));
  return step;
}

void Project::createDefaultSimulationSetup()
{
  clearSteps();

  auto pdbDownloader = addStep<Pipeline::PdbDownload::Step>();

  auto prepList = addPreparationPipeline();
  prepList[0]->getFileObjectConsumer()->connectTo(
    pdbDownloader->getFileObjectProvider()->provides()[0]
    );

  auto simulationList = addSimulationPipeline();
  simulationList[0]->getFileObjectConsumer()->connectTo(
    prepList[prepList.size() - 1]->getFileObjectProvider()->provides()[0]
    );

  for (auto simulation: simulationList)
  {
    simulation->getFileObjectConsumer()->connectTo(
      prepList[1]->getFileObjectProvider()->provides()[1]
      );
  }

  auto centerProtein = addStep<Pipeline::CenterProtein::Step>();
  centerProtein->getFileObjectConsumer()->connectTo(
    simulationList[simulationList.size() - 1]->getFileObjectProvider()->provides()[0]
    );
  centerProtein->getFileObjectConsumer()->connectTo(
    simulationList[simulationList.size() - 1]->getFileObjectProvider()->provides()[1]
    );
  auto smoothTrajectory = addStep<Pipeline::SmoothTrajectory::Step>();
  smoothTrajectory->getFileObjectConsumer()->connectTo(
    centerProtein->getFileObjectProvider()->provides()[0]
    );
  smoothTrajectory->getFileObjectConsumer()->connectTo(
    simulationList[simulationList.size() - 1]->getFileObjectProvider()->provides()[0]
    );
}

QJsonObject &operator<<(QJsonObject& out, const std::shared_ptr<Project> project)
{
  out["name"] = project->property("name").toString();

  QJsonArray connections;
  QJsonArray stepsArray;
  const auto& steps = project->getSteps();
  for (size_t stepIndex = 0; stepIndex < steps.size(); stepIndex++)
  {
    const auto& step = steps[stepIndex];
    QJsonObject stepObject;
    stepObject << step;
    stepsArray.append(stepObject);

    for (const auto& fileObject : step->getFileObjectConsumer()->getConnectedTo().values())
    {
      for (size_t sourceNodeIndex = 0; sourceNodeIndex < steps.size(); sourceNodeIndex++)
      {
        const auto& sourceStep = steps[sourceNodeIndex];
        int found = sourceStep->getFileObjectProvider()->provides().indexOf(fileObject);
        if (found > -1)
        {
          auto triple = QJsonArray({
            (int)stepIndex, (int)sourceNodeIndex, found
          });
          connections.append(triple);
          break;
        }
      }
    }
  }

  out["steps"] = stepsArray;
  out["connections"] = connections;

  return out;
}

QJsonObject &operator>>(QJsonObject &in, std::shared_ptr<Project> project)
{
  project->setProperty("name", in["name"].toString());

  project->clearSteps();

  if (in.contains("steps") && in["steps"].isArray())
  {
    QJsonArray steps = in["steps"].toArray();
    for (int index = 0; index < steps.size(); index++)
    {
      QJsonObject jsonStep = steps[index].toObject();
      QString type = jsonStep["type"].toString();
      Pipeline::Step::Pointer step = Pipeline::StepFactory::getInstance()
        ->createFromString(type, project);

      jsonStep >> step;
      project->addStep(std::move(step));
    }
  }

  if (in.contains("connections") && in["connections"].isArray())
  {
    auto& steps = project->getSteps();
    QJsonArray connections = in["connections"].toArray();
    for (int times = 0; times < 2; times ++)
    {
      for (int index = 0; index < connections.size(); index++)
      {
        QJsonArray connection = connections[index].toArray();
        const int targetIndex = connection[0].toInt();
        const int sourceIndex = connection[1].toInt();
        const int sourcePort = connection[2].toInt();

        steps[targetIndex]->getFileObjectConsumer()
          ->connectTo(steps[sourceIndex]->getFileObjectProvider()->provides()[sourcePort]);
      }
    }
  }

  return in;
}

}
