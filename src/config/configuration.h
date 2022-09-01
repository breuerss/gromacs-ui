#ifndef CONFIG_CONFIGURATION_H
#define CONFIG_CONFIGURATION_H

#include <QWidget>

namespace Config {

class Configuration {
public:
  virtual ~Configuration() = default;
  virtual QWidget* getUI() = 0;
};

}

#endif
