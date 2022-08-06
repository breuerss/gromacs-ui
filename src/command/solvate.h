#ifndef SOLVATE_H
#define SOLVATE_H

#include "executor.h"
#include "../model/systemsetup.h"
#include <memory>

namespace Command {

class Solvate : public Executor
{
public:
    explicit Solvate(std::shared_ptr<SystemSetup> systemSetup, QObject *parent = nullptr);
    void exec() override;

private:
    const std::shared_ptr<SystemSetup> systemSetup;
    static QString getWaterBoxFor(const QString& solvent);
};

}
#endif // SOLVATE_H
