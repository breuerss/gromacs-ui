#ifndef GROMACSTOOLEXECUTOR_H
#define GROMACSTOOLEXECUTOR_H

#include <memory>
#include "model/systemsetup.h"

class GromacsToolExecutor
{
public:
  // TODO make commands chainable without blocking UI
    static void execPdb2gmx(const std::shared_ptr<SystemSetup> systemSetup);
    static void execEditConf(const std::shared_ptr<SystemSetup> systemSetup);
    static void execSolvate(const std::shared_ptr<SystemSetup> systemSetup);
    static void execMdrun(const std::shared_ptr<Project> project, int step);
    static QString getWaterBoxFor(const QString& solvent);
};

#endif // GROMACSTOOLEXECUTOR_H
