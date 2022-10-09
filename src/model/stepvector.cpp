#include "stepvector.h"

namespace Model {

void StepVector::append(const std::shared_ptr<Pipeline::Step>& step)
{
  QList::append(step);
  stepAdded(step, size());
}

bool StepVector::removeOne(const std::shared_ptr<Pipeline::Step>& step)
{
  int at = QList::indexOf(step);
  stepRemoved(step, at);
  return QList::removeOne(step);
}

}

