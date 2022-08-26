#include "executorfileobjectprovider.h"

namespace Command {

ExecutorFileObjectProvider::ExecutorFileObjectProvider(
  std::shared_ptr<Model::Project> project,
  std::shared_ptr<Model::Simulation> step
  )
  : project(project)
    , step(step)
{
}
  
//QList<std::shared_ptr<FileObject>> ExecutorFileObjectProvider::provides()
//{
//  QList<std::shared_ptr<FileObject>> list;
//
//  return list;
//}


}
