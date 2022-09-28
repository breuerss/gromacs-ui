#ifndef MODEL_STEPSVECTOR_H
#define MODEL_STEPSVECTOR_H

#include "../pipeline/step.h"
#include <QObject>
#include <QList>
#include <memory>

namespace Model {

class StepVector: public QObject, public QList<std::shared_ptr<Pipeline::Step>> {
  Q_OBJECT
public:
  void append(const std::shared_ptr<Pipeline::Step>& step);
  bool removeOne(const std::shared_ptr<Pipeline::Step>& step);

signals:
  void stepAdded(std::shared_ptr<Pipeline::Step> step, int at);
  void stepRemoved(std::shared_ptr<Pipeline::Step> step, int at);

};
}


#endif
