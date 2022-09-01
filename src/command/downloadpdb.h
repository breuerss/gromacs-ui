#ifndef COMMAND_DOWNLOADPDB_H
#define COMMAND_DOWNLOADPDB_H

#include "executor.h"
#include <memory>

class PdbDownloader;
namespace Model {
class Project;
}

namespace Command {

class InputFileNameGenerator;

class DownloadPdb : public Executor
{
public:
  explicit DownloadPdb();
  void doExecute() override;
  bool canExecute() const override;
  void setFileNameGenerator(std::shared_ptr<InputFileNameGenerator> newfileNameGenerator);

private:
  std::shared_ptr<InputFileNameGenerator> fileNameGenerator;
  std::shared_ptr<PdbDownloader> downloader;
  QString getPdbCode() const;
};

}

#endif // RUNSIMULATION_H
