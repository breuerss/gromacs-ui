#ifndef PIPELINE_PDBDOWNLOAD_STEP_H
#define PIPELINE_PDBDOWNLOAD_STEP_H

#include "../step.h"

namespace Model {
class Project;
}

namespace Pipeline { namespace PdbDownload {

class Step : public Pipeline::Step {

public:
  Step(std::shared_ptr<Model::Project> project = nullptr);
  QString getName() const override;

  QString getType() const override;
  static Step::Pointer create(std::shared_ptr<Model::Project>);

private:
  static bool registered;
  static QString type;
};

} }

#endif
