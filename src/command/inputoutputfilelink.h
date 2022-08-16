#ifndef CHAINABLEEXECUTOR_H
#define CHAINABLEEXECUTOR_H

#include <memory>
#include <QString>

namespace Command {

class InputOutputFileLink
{
public:
  InputOutputFileLink() = default;
  virtual ~InputOutputFileLink() = default;

  void setPreviousLink(std::shared_ptr<InputOutputFileLink> newPreviousLink);
  std::shared_ptr<InputOutputFileLink> getPreviousLink() const;
  QString getInputFilename() const;

  virtual QString getOutputFilename() const = 0;

private:
  std::shared_ptr<InputOutputFileLink> previousLink;
};

}

#endif

