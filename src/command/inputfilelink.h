#ifndef COMMAND_INPUTFILELINK_H
#define COMMAND_INPUTFILELINK_H

#include "inputoutputfilelink.h"
#include <QString>

namespace Command {

class InputFileLink : public InputOutputFileLink
{
public:
  InputFileLink(const QString& filename);
  QString getOutputFilename() const override;

private:
  const QString filename;
};
}

#endif
