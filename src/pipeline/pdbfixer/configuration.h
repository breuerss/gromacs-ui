#ifndef PIPELINE_PDBFIXER_CONFIGURATION_H
#define PIPELINE_PDBFIXER_CONFIGURATION_H

#include "../../config/configuration.h"
#include <QString>

namespace Pipeline { namespace PdbFixer {

class Configuration
: public Config::Configuration
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
