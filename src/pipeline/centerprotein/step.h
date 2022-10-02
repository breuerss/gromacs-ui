#ifndef PIPELINE_CENTERPROTEIN_STEP_H
#define PIPELINE_CENTERPROTEIN_STEP_H

#include "../factoryregistration.h"

namespace Model {
class Project;
}

namespace Pipeline {

namespace CenterProtein {

class Step : public FactoryRegistration<Step> {

public:
  Step(std::shared_ptr<Model::Project> project = nullptr);
};

} }

#endif
