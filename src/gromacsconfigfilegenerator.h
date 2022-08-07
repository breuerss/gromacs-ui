#ifndef GROMACSCONFIGFILEGENERATOR_H
#define GROMACSCONFIGFILEGENERATOR_H

#include <memory>
#include <QString>
#include <QTextStream>
#include "model/step.h"

class GromacsConfigFileGenerator
{
public:
    static void generate(std::shared_ptr<Step> step, const QString& fileName);

private:
    static void writeLine(QTextStream& writer, const QString& key, const QString& value);
};

#endif // GROMACSCONFIGFILEGENERATOR_H