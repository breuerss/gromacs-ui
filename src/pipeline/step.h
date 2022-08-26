#ifndef PIPELINE_STEP_H
#define PIPELINE_STEP_H

#include "../command/fileobjectconsumer.h"
#include <QString>

namespace Pipeline {

class Step {
public:
  Step(const QMap<Command::FileObject::Category, QList<Command::FileObject::Type>>& requiresMap);
  virtual QString getName() const = 0;
  const Command::FileObjectConsumer fileObjectConsumer;
};

}

#endif
