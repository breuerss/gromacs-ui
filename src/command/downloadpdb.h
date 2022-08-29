#ifndef COMMAND_DOWNLOADPDB_H
#define COMMAND_DOWNLOADPDB_H

#include "executor.h"
#include <memory>
#include <QFileSystemWatcher>
#include "../pipeline/step.h"
#include "../pdbdownloader.h"

namespace Model {
class Project;
}

namespace Command {

class DownloadPdb : public Executor, public Pipeline::Step
{
public:
  explicit DownloadPdb(
    std::shared_ptr<Model::Project> project,
    QObject *parent = nullptr);
  void doExecute() override;
  QString getName() const override;

private:
  std::shared_ptr<Model::Project> project;
  std::shared_ptr<PdbDownloader> downloader;
};

}

#endif // RUNSIMULATION_H
