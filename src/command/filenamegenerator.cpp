#include "filenamegenerator.h"

namespace Command {

FileNameGenerator::FileNameGenerator(
  std::shared_ptr<Model::Project> newProject)
  : project(newProject)
{
}
 
void FileNameGenerator::setConfiguration(std::shared_ptr<Config::Configuration> newConfig)
{
  configuration = newConfig;
}
}
