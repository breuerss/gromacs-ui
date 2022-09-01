#ifndef PIPELINE_PDBDOWNLOAD_H
#define PIPELINE_PDBDOWNLOAD_H

#include "step.h"
#include "../model/project.h"

namespace Pipeline {

class PdbDownload : public Step {
public:
  PdbDownload(std::shared_ptr<Model::Project> project);
  QString getName() const override;
};

}

#endif
