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

private:
  static void writeLine(QTextStream& writer, const QString& key, const QString& value);
};

#endif // GROMACSCONFIGFILEGENERATOR_H
