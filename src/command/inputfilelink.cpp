#include "inputfilelink.h"

namespace Command {

InputFileLink::InputFileLink(const QString& filename)
  : filename(filename)
{
}

QString InputFileLink::getOutputFilename() const
{
  return filename;
}

}
