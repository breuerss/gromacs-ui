#include "step.h"

namespace Pipeline {
Step::Step(
  const QMap<Command::FileObject::Category, QList<Command::FileObject::Type>>& requiresMap
  )
  : fileObjectConsumer(requiresMap)
{
}

}

