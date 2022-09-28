#ifndef UNDO_REDO_ADD_STEP_COMMAND_H
#define UNDO_REDO_ADD_STEP_COMMAND_H

#include <QUndoCommand>
#include <memory>

namespace Model {
class Project;
}

namespace Pipeline {
class Step;
}

namespace UndoRedo {

class AddStepCommand : public QUndoCommand
{
public:
    AddStepCommand(
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
