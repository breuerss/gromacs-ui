#ifndef PIPELINE_FITTOINPUT_FILENAMEGENERATOR_H
#define PIPELINE_FITTOINPUT_FILENAMEGENERATOR_H

#include "../../command/filenamegenerator.h"

namespace Pipeline { namespace FitToInput {

class FileNameGenerator : public ::Command::FileNameGenerator
{
public:
  using ::Command::FileNameGenerator::FileNameGenerator;

  QString getFileNameFor(::Command::FileObject::Type type) const override;
};

} }

#endif
