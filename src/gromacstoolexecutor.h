#ifndef GROMACSTOOLEXECUTOR_H
#define GROMACSTOOLEXECUTOR_H

#include <memory>
#include "systemsetup.h"

class GromacsToolExecutor
{
public:
    static void execPdb2gmx(const std::shared_ptr<SystemSetup> systemSetup);
    static void execEditConf(const std::shared_ptr<SystemSetup> systemSetup);
    static void execSolvate(const std::shared_ptr<SystemSetup> systemSetup);
    static QString getWaterBoxFor(const QString& solvent);
};

#endif // GROMACSTOOLEXECUTOR_H
