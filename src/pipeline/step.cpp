#include "step.h"

namespace Pipeline {
Step::Step(
  const QMap<Command::FileObject::Category, QList<Command::FileObject::Type>>& requiresMap,
  const QList<Command::FileObject::Type> providesList,
  Config::Type configuration
  )
  : fileObjectConsumer(requiresMap)
  , fileObjectProvider(providesList)
  , configuration(configuration)
{
}

}

