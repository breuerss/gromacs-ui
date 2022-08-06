#ifndef CREATEGROMACSMODEL_H
#define CREATEGROMACSMODEL_H

#include "executor.h"
#include "../model/systemsetup.h"
#include <memory>

namespace Command {

class CreateGromacsModel : public Executor
{
public:
    explicit CreateGromacsModel(std::shared_ptr<SystemSetup> systemSetup, QObject *parent = nullptr);
    void exec() override;

private:
    const std::shared_ptr<SystemSetup> systemSetup;
};

}

#endif // CREATEGROMACSMODEL_H
