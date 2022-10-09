#include "removestepcommand.h"
#include <QObject>

#include "../model/project.h"

namespace UndoRedo {

RemoveStepCommand::RemoveStepCommand(
  std::shared_ptr<Pipeline::Step>&& newStep,
  Model::Project* newProject,
  QUndoCommand* parent)
  : QUndoCommand(QObject::tr("Remove step"), parent)
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
