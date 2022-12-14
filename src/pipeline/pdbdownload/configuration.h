#ifndef CONFIG_PDB_H
#define CONFIG_PDB_H

#include "../../config/configuration.h"
#include <QString>
#include <memory>

namespace Pipeline { namespace PdbDownload {

class Configuration
: public Config::Configuration
  , public std::enable_shared_from_this<Configuration>
{
  Q_OBJECT
public:
  Configuration();

  Q_PROPERTY(QString pdbCode MEMBER pdbCode NOTIFY pdbCodeChanged);
  QWidget* getUI() override;
  QString toString() override;

signals:
  void pdbCodeChanged();

private:
  QString pdbCode;
};

} }

#endif
