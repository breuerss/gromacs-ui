#ifndef GROMACSCONFIGFILEGENERATOR_H
#define GROMACSCONFIGFILEGENERATOR_H

#include <memory>
#include <type_traits>
#include <QString>
#include <QTextStream>
#include "config/simulation.h"

class GromacsConfigFileGenerator
{
public:
  GromacsConfigFileGenerator(Config::Simulation*);
  void generate(const QString& fileName);
  static std::shared_ptr<Config::Simulation> createFrom(const QString& fileName);
  void setFromMdpFile(const QString& fileName);
  void setFromTprFile(const QString& fileName);

private:
  template<typename T>
  QString getValueFromModelImpl(const QString& key, std::true_type)
  {
    return QString::number(configuration->property(optionsMap[key].toStdString().c_str()).value<T>());
  }

  template<typename T>
  QString getValueFromModelImpl(const QString& key, std::false_type)
  {
    return toString(configuration->property(optionsMap[key].toStdString().c_str()).value<T>());
  }

  template<typename T>
  QString getValueFromModel(const QString& key)
  {
    return getValueFromModelImpl<T>(key, std::integral_constant<bool, ((
        std::is_integral<T>::value && !std::is_enum<T>::value && !std::is_same<T, bool>::value
        ) || std::is_floating_point<T>::value)
      >());
  }

  void writeLine(QTextStream& writer, const QString& key, const QString& value);
  template<typename T>
  void writeLine(
    QTextStream& writer,
    const QString& key
    )
  {
    writeLine(writer, key, getValueFromModel<T>(key));
  }

  static const QMap<QString, QString> optionsMap;
  static const QMap<QString, std::function<QVariant(const QString&)>> conversionMap;
  static const QList<QString> temperatureCouplingOptions;
  static QVariant createValueFrom(const QString& option, const QString& inputValueString);

private:
  Config::Simulation* configuration;
};

#endif // GROMACSCONFIGFILEGENERATOR_H
