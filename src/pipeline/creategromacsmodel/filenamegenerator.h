#include "../../command/filenamegenerator.h"

namespace Pipeline { namespace CreateGromacsModel {

class FileNameGenerator : public ::Command::FileNameGenerator
{
public:
  using ::Command::FileNameGenerator::FileNameGenerator;

  QString getFileNameFor(::Command::FileObject::Type type) const override;
};

} }
