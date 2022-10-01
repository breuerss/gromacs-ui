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

void Runner::handleNextSteps(
  std::shared_ptr<Pipeline::Step> step,
  std::shared_ptr<Model::Project> project
  )
{  
  auto command = step->getCommand();
  conns << connect(
    command.get(), &Command::Executor::finished,
    [this, step, project] () {
      running--;
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
      if (std::holds_alternative<FileObject::Pointer>(data))
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
