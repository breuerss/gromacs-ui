#ifndef CONFIG_CONFIGURATION_H
#define CONFIG_CONFIGURATION_H

#include "../model/serializable.h"

#include <QWidget>
#include <memory>

namespace Config {

class Configuration
: public Model::Serializable
{
public:
  typedef std::shared_ptr<Configuration> Pointer;
  virtual ~Configuration() = default;
  virtual QWidget* getUI() = 0;
  virtual QString toString() { return QString(); };
};

}

#endif
