#ifndef PIPELINE_CREATEBOX_CONFIGURATION_H
#define PIPELINE_CREATEBOX_CONFIGURATION_H

#include "../../model/serializable.h"
#include "../../config/configuration.h"
#include <QString>
#include <memory>

namespace Pipeline { namespace CreateBox {

class Configuration
: public Model::Serializable
, public Config::Configuration
{
  Q_OBJECT
public:
  enum class BoxType : int {
    None = 0,
    Cubic,
    Octahedron,
    Dodecahedron,
  };
  Q_ENUM(BoxType);

  Configuration();

  Q_PROPERTY(BoxType boxType MEMBER boxType NOTIFY boxTypeChanged)
  Q_PROPERTY(double distance MEMBER distance NOTIFY distanceChanged)

  QWidget* getUI() override;

signals:
  void boxTypeChanged(const BoxType&);
  void distanceChanged(double);

private:
  BoxType boxType = BoxType::Dodecahedron;
  double distance = 1.0;
};

QString toString(Configuration::BoxType type);

} }

#endif
