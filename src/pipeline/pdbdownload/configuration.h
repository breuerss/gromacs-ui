#ifndef CONFIG_PDB_H
#define CONFIG_PDB_H

#include "../../model/serializable.h"
#include "../../config/configuration.h"
#include <QString>
#include <memory>

namespace Pipeline { namespace PdbDownload {

class Configuration
: public Model::Serializable
  , public Config::Configuration
  , public std::enable_shared_from_this<Configuration>
{
  Q_OBJECT
public:
  Configuration() = default;

  Q_PROPERTY(QString pdbCode MEMBER pdbCode NOTIFY pdbCodeChanged);
  QWidget* getUI() override;

signals:
  void pdbCodeChanged();

private:
  QString pdbCode;
};

} }

#endif
