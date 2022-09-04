#ifndef PIPELINE_SOLVATE_CONFIGURATION_H
#define PIPELINE_SOLVATE_CONFIGURATION_H

#include "../../model/serializable.h"
#include "../../config/configuration.h"
#include <QString>
#include <memory>

namespace Pipeline { namespace Solvate {

class Configuration
: public Model::Serializable
, public Config::Configuration
{
  Q_OBJECT
public:
  Configuration() = default;

  QWidget* getUI() override;

};

} }

#endif
