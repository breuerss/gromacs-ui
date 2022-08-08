#ifndef SIMULATIONTYPE_H
#define SIMULATIONTYPE_H

#include <QObject>
#include <QDataStream>

namespace Model {
Q_NAMESPACE

enum class SimulationType : int {
  None = 0,
  Minimisation,
  NVE,
  NVT,
  NPT
};

}

Q_DECLARE_METATYPE(Model::SimulationType)

#endif // SIMULATIONTYPE_H
