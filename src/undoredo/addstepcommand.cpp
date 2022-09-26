#include "addstepcommand.h"

#include <QObject>
#include <QDebug>

namespace UndoRedo {

AddStepCommand::AddStepCommand(
  std::shared_ptr<Pipeline::Step>&& newStep,
  Model::Project* newProject,
  QUndoCommand* parent)
  : QUndoCommand(QObject::tr("Add step"), parent)
    , project(newProject)
    , step(newStep)
{
}

void AddStepCommand::undo()
{
  project->getSteps().removeOne(step);
}

void AddStepCommand::redo()
{
  project->getSteps().append(step);
}

}
