#ifndef CONFIG_CONFIGURATION_H
#define CONFIG_CONFIGURATION_H

#include <QWidget>
#include "../model/serializable.h"

namespace Config {

class Configuration
: public Model::Serializable
{
public:
  virtual ~Configuration() = default;
  virtual QWidget* getUI() = 0;
};

}

#endif
