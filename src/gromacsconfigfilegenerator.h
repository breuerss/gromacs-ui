#ifndef GROMACSCONFIGFILEGENERATOR_H
#define GROMACSCONFIGFILEGENERATOR_H

#include <memory>
#include <QString>
#include <QTextStream>

namespace Model {
class Simulation;
}

class GromacsConfigFileGenerator
{
public:
  static void generate(std::shared_ptr<Model::Simulation> step, const QString& fileName);
  static std::shared_ptr<Model::Simulation> createFrom(const QString& fileName);
  static void setFromMdpFile(std::shared_ptr<Model::Simulation>, const QString& fileName);
  static void setFromTprFile(std::shared_ptr<Model::Simulation>, const QString& fileName);

private:
  static void writeLine(QTextStream& writer, const QString& key, const QString& value);
  static const QMap<QString, QString> optionsMap;
  static const QMap<QString, std::function<QVariant(const QString&)>> conversionMap;
  static const QList<QString> temperatureCouplingOptions;
  static QVariant createValueFrom(const QString& option, const QString& inputValueString);
};

#endif // GROMACSCONFIGFILEGENERATOR_H
