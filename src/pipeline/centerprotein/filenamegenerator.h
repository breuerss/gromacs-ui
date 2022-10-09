#ifndef PIPELINE_CENTERPROTEIN_FILENAMEGENERATOR_H
#define PIPELINE_CENTERPROTEIN_FILENAMEGENERATOR_H

#include "../../command/filenamegenerator.h"

namespace Pipeline { namespace CenterProtein {

class FileNameGenerator : public ::Command::FileNameGenerator
{
public:
  using ::Command::FileNameGenerator::FileNameGenerator;

  QString getFileNameFor(::Command::FileObject::Type type) const override;
};

} }

#endif
