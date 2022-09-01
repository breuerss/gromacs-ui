#ifndef COMMAND_FILENAMEGENERATOR_H
#define COMMAND_FILENAMEGENERATOR_H

#include <memory>
#include "fileobject.h"

namespace Model {
class Project;
}

namespace Config {
class Configuration;
}

namespace Command {

class FileNameGenerator {
public:
  FileNameGenerator(std::shared_ptr<Model::Project>);
  virtual ~FileNameGenerator() = default;

  void setConfiguration(std::shared_ptr<Config::Configuration> newConfig);
  virtual QString getFileNameFor(FileObject::Type type) const = 0;

protected:
  std::shared_ptr<Model::Project> project;
  std::shared_ptr<Config::Configuration> configuration;
};
}

#endif
