#ifndef CREATEBOX_H
#define CREATEBOX_H

#include "executor.h"
#include "../model/systemsetup.h"
#include <memory>

namespace Command {

class CreateBox : public Executor
{
public:
    explicit CreateBox(std::shared_ptr<SystemSetup> systemSetup, QObject *parent = nullptr);
    void exec() override;

private:
    const std::shared_ptr<SystemSetup> systemSetup;
};

}

#endif // CREATEBOX_H
