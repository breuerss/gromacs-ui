#ifndef CREATEBOX_H
#define CREATEBOX_H

#include "executor.h"
#include <memory>

namespace Model {
class SystemSetup;
}

namespace Command {

class CreateBox : public Executor
{
public:
    explicit CreateBox(std::shared_ptr<Model::SystemSetup> systemSetup, QObject *parent = nullptr);
    void exec() override;

private:
    const std::shared_ptr<Model::SystemSetup> systemSetup;
};

}

#endif // CREATEBOX_H
