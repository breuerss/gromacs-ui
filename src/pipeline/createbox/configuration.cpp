#include "configuration.h"
#include "../ui/createboxsettings.h"
#include <memory>
#include <QMetaProperty>

namespace Pipeline { namespace CreateBox {

Configuration::Configuration()
{
  static bool registered = false;
  if (!registered)
  {
    qRegisterMetaType<BoxType>("Configuration::BoxType");
    qRegisterMetaTypeStreamOperators<int>("Configuration::BoxType");
    registered = true;
  }

  connectAllSignals();
}

QString Configuration::toString()
{
  return tr("Box") + ": " + CreateBox::toString(boxType);
}

QWidget* Configuration::getUI()
{
  return new CreateBoxSettings(this);
}

QString toString(Configuration::BoxType type)
{
  using BoxType = Configuration::BoxType;
  const static QMap<BoxType, QString> map = {
    { BoxType::Cubic, "cubic" },
    { BoxType::Octahedron, "octahedron" },
    { BoxType::Dodecahedron, "dodecahedron" },
  };

  return map[type];
}


} }
