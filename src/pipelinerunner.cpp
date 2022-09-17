#include "pipelinerunner.h"
#include "projectmanager.h"
#include "command/fileobjectconsumer.h"
#include "command/fileobjectprovider.h"
#include "command/fileobject.h"
#include "src/command/executor.h"
#include <memory>
#include <qdebug.h>

PipelineRunner* PipelineRunner::getInstance()
{
  static PipelineRunner instance;

  return &instance;
}

void PipelineRunner::startPipeline()
{
  auto project = ProjectManager::getInstance()->getCurrentProject();

  auto startingSteps = getStartingSteps(project);
  for (auto step : startingSteps)
  {
    handleNextSteps(step, project);
  }
}

void PipelineRunner::handleNextSteps(
  std::shared_ptr<Pipeline::Step> step,
  std::shared_ptr<Model::Project> project
  )
{  
  auto command = step->getCommand();
  conns << connect(
    command.get(), &Command::Executor::finished,
    [this, step, project] () {
      running--;
      auto nextSteps = getNextStepsFor(step, project);
      for (auto step : nextSteps)
      {
        handleNextSteps(step, project);
      }

      if (running == 0)
      {
        qDebug() << "finished";
        for (auto conn : conns)
        {
          disconnect(conn);
        }
      }
    });

  command->exec();
  running++;
}

QList<std::shared_ptr<Pipeline::Step>>
PipelineRunner::getStartingSteps(std::shared_ptr<Model::Project> project) const
{
  QList<std::shared_ptr<Pipeline::Step>> startingSteps;
  for (auto step : project->getSteps())
  {
    if (step->getFileObjectConsumer()->getConnectedTo().size() == 0)
    {
      startingSteps << step;
    }
  }

  return startingSteps;
}

QList<std::shared_ptr<Pipeline::Step>>
PipelineRunner::getNextStepsFor(
  std::shared_ptr<Pipeline::Step> step,
  std::shared_ptr<Model::Project> project
  ) const
{

  using FileObject = Command::FileObject;
  using Type = Command::FileObject::Type;
  auto outputFiles = step->getFileObjectProvider()->provides();
  QList<std::shared_ptr<FileObject>> relevantOutputFiles;
  for (auto file: outputFiles)
  {
    if (
      file->type == Type::GRO ||
      file->type == Type::PDB ||
      file->type == Type::XTC ||
      file->type == Type::EDR ||
      file->type == Type::TRR
      )
    {
      relevantOutputFiles << file;
    }
  }

  QList<std::shared_ptr<Pipeline::Step>> nextSteps;
  for (auto file : relevantOutputFiles)
  {
    for (const auto& step : project->getSteps())
    {
      if (step->getFileObjectConsumer()->getConnectedTo().values().contains(file) &&
          !nextSteps.contains(step)
          )
      {
        nextSteps << step;
      }
    }
  }

  return nextSteps;
}


