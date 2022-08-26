#ifndef COMMAND_EXECUTORFILEOBJECTPROVIDER_H
#define COMMAND_EXECUTORFILEOBJECTPROVIDER_H

#include "fileobjectprovider.h"
#include "src/model/project.h"
#include "../config/simulation.h"
#include <memory>

namespace Command {

class ExecutorFileObjectProvider// : public FileObjectProvider
{
public:
  ExecutorFileObjectProvider(
    std::shared_ptr<Model::Project> project,
    std::shared_ptr<Config::Simulation> step
    );
  
  //virtual QList<std::shared_ptr<FileObject>> provides() const override;

private:
  std::shared_ptr<Model::Project> project;
  std::shared_ptr<Config::Simulation> step;
};

}

#endif
