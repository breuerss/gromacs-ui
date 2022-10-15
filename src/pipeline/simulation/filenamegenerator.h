#ifndef PIPELINE_SIMULATION_FILENAMEGENERATOR_H
#define PIPELINE_SIMULATION_FILENAMEGENERATOR_H

#include "../../command/filenamegenerator.h"

namespace Pipeline { namespace Simulation {

class FileNameGenerator : public ::Command::FileNameGenerator
{
public:
  using ::Command::FileNameGenerator::FileNameGenerator;

  QString getFileNameFor(::Command::FileObject::Type type) const override;
};

} }

#endif
