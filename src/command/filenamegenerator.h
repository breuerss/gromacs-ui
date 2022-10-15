#ifndef COMMAND_FILENAMEGENERATOR_H
#define COMMAND_FILENAMEGENERATOR_H

#include <memory>
#include <QString>
#include "fileobject.h"

namespace Model {
class Project;
}

namespace Config {
class Configuration;
}

namespace Command {

class FileObjectConsumer;

class FileNameGenerator {
public:
  FileNameGenerator(std::shared_ptr<Model::Project>);
  virtual ~FileNameGenerator() = default;

  void setConfiguration(const Config::Configuration* newConfig);
  void setFileObjectConsumer(std::shared_ptr<FileObjectConsumer> newConsumer);
  virtual QString getFileNameFor(FileObject::Type type) const = 0;

protected:
  std::shared_ptr<Model::Project> project;
  const Config::Configuration* configuration;
  std::shared_ptr<FileObjectConsumer> fileObjectConsumer;
};
}

#endif
