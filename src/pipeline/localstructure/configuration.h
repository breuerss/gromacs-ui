#ifndef CONFIG_PDB_H
#define CONFIG_PDB_H

#include "../../config/configuration.h"
#include <QString>
#include <memory>

namespace Pipeline { namespace LocalStructure {

class Configuration
: public Config::Configuration
  , public std::enable_shared_from_this<Configuration>
{
  Q_OBJECT
public:
  Configuration();

  Q_PROPERTY(QString fileName MEMBER fileName NOTIFY fileNameChanged);
  QWidget* getUI() override;
  QString toString() override;

signals:
  void fileNameChanged();

private:
  QString fileName;
};

} }

#endif
