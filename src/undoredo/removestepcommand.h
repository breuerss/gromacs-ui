#ifndef UNDO_REDO_REMOVE_STEP_COMMAND_H
#define UNDO_REDO_REMOVE_STEP_COMMAND_H

#include <QUndoCommand>
#include "src/model/project.h"
#include "src/pipeline/step.h"

namespace UndoRedo {

class RemoveStepCommand : public QUndoCommand
{
public:
    RemoveStepCommand(
      std::shared_ptr<Pipeline::Step>&& step,
      Model::Project* project,
      QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Model::Project* project;
    std::shared_ptr<Pipeline::Step> step;
};

}

#endif
