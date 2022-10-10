#include "runner.h"
#include "../projectmanager.h"
#include "../command/fileobjectconsumer.h"
#include "../command/fileobjectprovider.h"
#include "../command/fileobject.h"
#include "../command/executor.h"
#include <memory>
#include <qdebug.h>

namespace Pipeline {

Runner* Runner::getInstance()
{
  static Runner instance;

  return &instance;
}

void Runner::startPipeline()
{
  auto project = ProjectManager::getInstance()->getCurrentProject();

  auto startingSteps = getStartingSteps(project);
  for (auto step : startingSteps)
  {
    handleNextSteps(step, project);
  }
}

void Runner::stopPipeline()
{
  for (auto command: runningSteps.keys())
  {
    cleanUpCommand(command);
  }
}

bool Runner::isRunning()
{
  return runningSteps.size() > 0;
}

void Runner::cleanUpCommand(std::shared_ptr<Command::Executor> command)
{
  disconnect(runningSteps[command]);
  runningSteps.remove(command);
  if (command->isRunning())
  {
    command->stop();
  }
  runningChanged(isRunning());
}

void Runner::handleNextSteps(
  std::shared_ptr<Pipeline::Step> step,
  std::shared_ptr<Model::Project> project
  )
{  
  auto command = step->getCommand();
  if (!command)
  {
    return;
  }
  auto conn = connect(
    command.get(), &Command::Executor::finished,
    [this, step, project, command] (bool successful) {
      if (successful)
      {
        auto nextSteps = getNextStepsFor(
          step, project,
          {
            Type::GRO,
            Type::PDB,
            Type::XTC,
            Type::TOP,
            Type::EDR,
            Type::TRR,
          });
        for (auto step : nextSteps)
        {
          handleNextSteps(step, project);
        }
      }

      cleanUpCommand(command);
    });

  runningSteps[command] = conn;

  command->exec();

  runningChanged(isRunning());
}

QList<std::shared_ptr<Pipeline::Step>>
Runner::getStartingSteps(std::shared_ptr<Model::Project> project) const
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
Runner::getNextStepsFor(
  std::shared_ptr<Pipeline::Step> step,
  std::shared_ptr<Model::Project> project,
  const QList<Command::FileObject::Type> relevantTypes
  )
{

  auto outputData = step->getFileObjectProvider()->provides();
  using FileObject = Command::FileObject;
  auto relevantOutputFiles = outputData;

  if (relevantTypes.size())
  {
    for (const auto& data: outputData)
    {
      if (Command::isSet<FileObject::Pointer>(data))
      {
        auto file = std::get<FileObject::Pointer>(data);
        if (!relevantTypes.contains(file->type))
        {
          relevantOutputFiles.removeAll(file);
        }
      }
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

}
