#include "filenamegenerator.h"

namespace Command {

class InputFileNameGenerator : public FileNameGenerator
{
public:
  using FileNameGenerator::FileNameGenerator;

  QString getFileNameFor(FileObject::Type type) const override;
};

}
