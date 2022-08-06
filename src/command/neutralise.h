#ifndef NEUTRALISE_H
#define NEUTRALISE_H

#include "executor.h"
#include "../model/systemsetup.h"

#include <memory>

namespace Command {

class Neutralise : public Executor
{
public:
    explicit Neutralise(std::shared_ptr<SystemSetup> newSystemSetup, QObject *parent = nullptr);
    void exec() override;

private:
    const std::shared_ptr<SystemSetup> systemSetup;
};

}

#endif // NEUTRALISE_H
