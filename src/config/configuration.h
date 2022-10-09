#ifndef CONFIG_CONFIGURATION_H
#define CONFIG_CONFIGURATION_H

#include "../model/serializable.h"

#include <QWidget>
#include <QJsonObject>
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
  virtual void toObject(QJsonObject& out) {
    out << this;
  };
  virtual void fromObject(QJsonObject& in) {
    in >> this;
  };
};

}

#endif
