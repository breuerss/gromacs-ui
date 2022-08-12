#include "inputoutputfilelink.h"

namespace Command {

void InputOutputFileLink::setPreviousLink(std::shared_ptr<InputOutputFileLink> newPreviousLink)
{
  previousLink = newPreviousLink;
}

std::shared_ptr<InputOutputFileLink> InputOutputFileLink::getPreviousLink() const
{
  return previousLink;
}

QString InputOutputFileLink::getInputFilename() const
{
  auto prevCommand = getPreviousLink();
  if (!prevCommand)
  {
    throw std::runtime_error("Previous command missing.");
  }

  return prevCommand->getOutputFilename();
}

}
