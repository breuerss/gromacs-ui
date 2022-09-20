#include "removestepcommand.h"

namespace UndoRedo {

RemoveStepCommand::RemoveStepCommand(
  std::shared_ptr<Pipeline::Step>&& newStep,
  Model::Project* newProject,
  QUndoCommand* parent)
  : QUndoCommand(parent)
    , project(newProject)
    , step(newStep)
{
}

void RemoveStepCommand::undo()
{
  project->getSteps().append(step);
}

void RemoveStepCommand::redo()
{
  project->getSteps().removeOne(step);
}

}
