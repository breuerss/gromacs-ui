#include "filenamegenerator.h"

namespace Command {

FileNameGenerator::FileNameGenerator(
  std::shared_ptr<Model::Project> newProject)
  : project(newProject)
{
}
 
void FileNameGenerator::setConfiguration(const Config::Configuration* newConfig)
{
  configuration = newConfig;
}

void FileNameGenerator::setFileObjectConsumer(std::shared_ptr<FileObjectConsumer> newConsumer)
{
  fileObjectConsumer = newConsumer;
}

}
