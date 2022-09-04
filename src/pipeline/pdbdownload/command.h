#ifndef COMMAND_DOWNLOADPDB_H
#define COMMAND_DOWNLOADPDB_H

#include "../../command/executor.h"
#include <memory>
#include <qobjectdefs.h>

class PdbDownloader;
namespace Model {
class Project;
}


namespace Pipeline { namespace PdbDownload {

class FileNameGenerator;

class Command : public ::Command::Executor
{
public:
  explicit Command();
  void doExecute() override;
  bool canExecute() const override;

private:
  std::shared_ptr<PdbDownloader> downloader;
  QString getPdbCode() const;
  QMetaObject::Connection configConnection;
};

} }

#endif // RUNSIMULATION_H